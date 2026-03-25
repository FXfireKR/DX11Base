#include "pch.h"
#include "CChunkMeshBuilder.h"
#include "CChunkWorld.h"

bool CChunkMeshBuilder::BuildSectionMeshes(const CChunkWorld& world, int cx, int sy, int cz
    , const CChunkSection& section, ChunkSectionMeshSet& outMeshes) const
{
    outMeshes.Clear();

    const int baseWx = cx * CHUNK_SIZE_X;
    const int baseWy = sy * CHUNK_SECTION_SIZE;
    const int baseWz = cz * CHUNK_SIZE_Z;

    for (int ly = 0; ly < CHUNK_SECTION_SIZE; ++ly)
    {
        for (int lz = 0; lz < CHUNK_SIZE_Z; ++lz)
        {
            for (int lx = 0; lx < CHUNK_SIZE_X; ++lx)
            {
                const BlockCell cell = section.GetBlock(lx, ly, lz);

                if (cell.IsAir())
                    continue;

                if (BlockDB.GetRenderLayer(cell.blockID) == BLOCK_RENDER_LAYER::INVISIBLE_LAYER)
                    continue;
    
                const int wx = baseWx + lx;
                const int wy = baseWy + ly;
                const int wz = baseWz + lz;
    
                _AppendBlockQuads(world, wx, wy, wz, lx, ly, lz, cell, outMeshes);
            }
        }
    }    
    return true;
}

bool CChunkMeshBuilder::_AppendBlockQuads(const CChunkWorld& world, int wx, int wy, int wz, int lx, int ly, int lz
    , const BlockCell& cell, ChunkSectionMeshSet& outMeshes) const
{
    ChunkMeshData* pTargetMesh = nullptr;

    switch (BlockDB.GetRenderLayer(cell.blockID))
    {
        case BLOCK_RENDER_LAYER::CUTOUT_LAYER:
        {
            pTargetMesh = &outMeshes.cutout;
        } break;

        case BLOCK_RENDER_LAYER::TRANSLUCENT_LAYER:
        {
            pTargetMesh = &outMeshes.translucent;
        } break;

        case BLOCK_RENDER_LAYER::OPAQUE_LAYER:
        default:
        {
            pTargetMesh = &outMeshes.opaque;
        } break;
    }

    vector<AppliedModel> vecModels;
    if (!BlockDB.GetAppliedModels(cell.blockID, cell.stateIndex, vecModels))
        return false;

    for (const AppliedModel& applied : vecModels)
    {
        const BakedModel* pBakedModel = BlockDB.FindBakedModel(applied.modelHash);
        if (!pBakedModel)
            continue;

        for (const BakedQuad& srcQuad : pBakedModel->quads)
        {
            BakedQuad quad = srcQuad;
            _ApplyModelRotation(quad, applied.x, applied.y);

            if (quad.bHasCullFace)
            {
                if (_ShouldCullFace(world, wx, wy, wz, cell, static_cast<FACE_DIR>(quad.cullFaceDir)))
                    continue;
            }

            _AppendQuad(world, quad, wx, wy, wz, lx, ly, lz, *pTargetMesh);
        }
    }
    
    return true;
}

bool CChunkMeshBuilder::_ShouldCullFace(const CChunkWorld& world, int wx, int wy, int wz, const BlockCell& cell, FACE_DIR dir) const
{
    XMINT3 n{};
    switch (dir)
    {
    case FACE_DIR::PX: n = { 1, 0, 0 }; break;
    case FACE_DIR::NX: n = { -1, 0, 0 }; break;
    case FACE_DIR::PY: n = { 0, 1, 0 }; break;
    case FACE_DIR::NY: n = { 0, -1, 0 }; break;
    case FACE_DIR::PZ: n = { 0, 0, 1 }; break;
    case FACE_DIR::NZ: n = { 0, 0, -1 }; break;
    default: break;
    }

    const BlockCell neighbor = world.GetBlock(wx + n.x, wy + n.y, wz + n.z);
    if (neighbor.IsAir())
        return false;

    const BLOCK_RENDER_LAYER curLayer = BlockDB.GetRenderLayer(cell.blockID);
    const BLOCK_RENDER_LAYER neighborLayer = BlockDB.GetRenderLayer(neighbor.blockID);

    // 단, 같은 translucent 블록끼리는 내부면 제거
    if (cell.blockID == neighbor.blockID &&
        cell.stateIndex == neighbor.stateIndex &&
        BlockDB.CanCullSameBlockFace(cell.blockID))
    {
        return true;
    }

    // translucent가 한쪽이라도 끼면 기본은 컬링하지 않음
    if (curLayer == BLOCK_RENDER_LAYER::TRANSLUCENT_LAYER ||
        neighborLayer == BLOCK_RENDER_LAYER::TRANSLUCENT_LAYER)
    {
        return false;
    }

    return BlockDB.IsFaceOccluder(neighbor.blockID);    
}

bool CChunkMeshBuilder::_AppendQuad(const CChunkWorld& world, const BakedQuad& quad
    , int wx, int wy, int wz, int lx, int ly, int lz, ChunkMeshData& outMesh) const
{
    AtlasRegion region{};

    // TODO: #1 Change String key to Hash key
    if (!BlockResDB.TryGetRegion(quad.debugTextureKey.c_str(), region))
        return false;

    const uint32_t baseIndex = static_cast<uint32_t>(outMesh.vertices.size());
    const XMFLOAT4 color = _ResolveQuadColor_DebugBlockLight(world, quad, wx, wy, wz);

    for (int i = 0; i < 4; ++i)
    {
        ChunkMeshVertex v{};
        v.position.x = quad.verts[i].pos.x + static_cast<float>(lx);
        v.position.y = quad.verts[i].pos.y + static_cast<float>(ly);
        v.position.z = quad.verts[i].pos.z + static_cast<float>(lz);
        v.normal = quad.verts[i].normal;
        v.color = color;
        v.uv = _RemapAtlasUV(region, quad.verts[i].uv);
        
        outMesh.vertices.push_back(v);
    }

    outMesh.indices.push_back(baseIndex + 0);
    outMesh.indices.push_back(baseIndex + 1);
    outMesh.indices.push_back(baseIndex + 2);
    outMesh.indices.push_back(baseIndex + 0);
    outMesh.indices.push_back(baseIndex + 2);
    outMesh.indices.push_back(baseIndex + 3);
    return true;
}

uint8_t CChunkMeshBuilder::_ResolveQuadBlockLight(const CChunkWorld& world, const BakedQuad& quad, int wx, int wy, int wz) const
{
    if (quad.bHasCullFace)
    {
        const XMINT3 n = FaceToNormalInt3(static_cast<FACE_DIR>(quad.cullFaceDir));
        return world.GetBlockLight(wx + n.x, wy + n.y, wz + n.z);
    }
    return world.GetBlockLight(wx, wy, wz);
}

XMFLOAT4 CChunkMeshBuilder::_ResolveQuadColor_DebugBlockLight(const CChunkWorld& world, const BakedQuad& quad, int wx, int wy, int wz) const
{
    const uint8_t light = _ResolveQuadBlockLight(world, quad, wx, wy, wz);
    const float blockLight = static_cast<float>(light) / 15.0f;

    const float brightness = 0.1f + (0.9f * blockLight);
    XMFLOAT4 tint = ResolveBlockTint(quad);

    return { tint.x* brightness, tint.y * brightness, tint.z * brightness, tint.w };
}

XMFLOAT3 CChunkMeshBuilder::_RotatePointByBlockState(const XMFLOAT3& p, uint8_t rotXDeg, uint8_t rotYDeg) const
{
    XMVECTOR v = XMVectorSet(p.x - 0.5f, p.y - 0.5f, p.z - 0.5f, 0.0f);

    if (rotXDeg != 0)
    {
        const float rx = XMConvertToRadians(static_cast<float>(rotXDeg));
        v = XMVector3Transform(v, XMMatrixRotationX(rx));
    }

    if (rotYDeg != 0)
    {
        const float ry = XMConvertToRadians(static_cast<float>(rotYDeg));
        v = XMVector3Transform(v, XMMatrixRotationY(ry));
    }

    XMFLOAT3 out{};
    XMStoreFloat3(&out, v);

    out.x += 0.5f;
    out.y += 0.5f;
    out.z += 0.5f;
    return out;
}

XMFLOAT3 CChunkMeshBuilder::_RotateNormalByBlockState(const XMFLOAT3& n, uint8_t rotXDeg, uint8_t rotYDeg) const
{
    XMVECTOR v = XMVectorSet(n.x, n.y, n.z, 0.0f);

    if (rotXDeg != 0)
    {
        const float rx = XMConvertToRadians(static_cast<float>(rotXDeg));
        v = XMVector3TransformNormal(v, XMMatrixRotationX(rx));
    }

    if (rotYDeg != 0)
    {
        const float ry = XMConvertToRadians(static_cast<float>(rotYDeg));
        v = XMVector3TransformNormal(v, XMMatrixRotationY(ry));
    }

    v = XMVector3Normalize(v);

    XMFLOAT3 out{};
    XMStoreFloat3(&out, v);
    return out;
}

FACE_DIR CChunkMeshBuilder::_RotateFaceDirY(FACE_DIR dir, uint8_t rotYDeg) const
{
    const int step = (rotYDeg / 90) & 3;
    FACE_DIR cur = dir;

    for (int i = 0; i < step; ++i)
    {
        switch (cur)
        {
        case FACE_DIR::PZ: cur = FACE_DIR::PX; break;
        case FACE_DIR::PX: cur = FACE_DIR::NZ; break;
        case FACE_DIR::NZ: cur = FACE_DIR::NX; break;
        case FACE_DIR::NX: cur = FACE_DIR::PZ; break;
        default: break; // PY / NY unchanged
        }
    }

    return cur;
}

void CChunkMeshBuilder::_ApplyModelRotation(BakedQuad& quad, uint8_t rotXDeg, uint8_t rotYDeg) const
{
    for (int i = 0; i < 4; ++i)
    {
        quad.verts[i].pos = _RotatePointByBlockState(quad.verts[i].pos, rotXDeg, rotYDeg);
        quad.verts[i].normal = _RotateNormalByBlockState(quad.verts[i].normal, rotXDeg, rotYDeg);
    }

    // 지금 wall_torch는 y 회전만 쓰므로 dir/cullFace도 y만 반영
    quad.dir = static_cast<uint8_t>(_RotateFaceDirY(static_cast<FACE_DIR>(quad.dir), rotYDeg));

    if (quad.bHasCullFace)
    {
        quad.cullFaceDir = static_cast<uint8_t>(
            _RotateFaceDirY(static_cast<FACE_DIR>(quad.cullFaceDir), rotYDeg));
    }
}

XMFLOAT4 CChunkMeshBuilder::ResolveBlockTint(const BakedQuad& quad) const
{
    if (quad.tintIndex < 0)
        return { 1.f, 1.f, 1.f, 1.f };

        // 나중에 biom grass color sampler
    return {0.55f, 0.74f, 0.32f, 1.f};
}

XMFLOAT2 CChunkMeshBuilder::_RemapAtlasUV(const AtlasRegion& region, const XMFLOAT2& localUV) const
{
    XMFLOAT2 outUV;
    outUV.x = region.u0 + (region.u1 - region.u0) * localUV.x;
    outUV.y = region.v0 + (region.v1 - region.v0) * localUV.y;
    return outUV;
}

