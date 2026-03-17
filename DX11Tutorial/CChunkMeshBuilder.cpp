#include "pch.h"
#include "CChunkMeshBuilder.h"
#include "CChunkWorld.h"


bool CChunkMeshBuilder::BuildSectionMesh(const CChunkWorld& world, int cx, int sy, int cz, const CChunkSection& section, ChunkMeshData& outMesh) const
{
    outMesh.Clear();

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
    
                const int wx = baseWx + lx;
                const int wy = baseWy + ly;
                const int wz = baseWz + lz;
    
                _AppendBlockQuads(world, wx, wy, wz, lx, ly, lz, cell, outMesh);
            }
        }
    }    
    return true;
}

bool CChunkMeshBuilder::_AppendBlockQuads(const CChunkWorld& world, int wx, int wy, int wz, int lx, int ly, int lz
    , const BlockCell& cell, ChunkMeshData& outMesh) const
{
    const BakedModel* pBakedModel = BlockDB.GetBakedModel(cell.blockID, cell.stateIndex);
    if (nullptr == pBakedModel)
        return false;

    for (const BakedQuad& quad : pBakedModel->quads)
    {
        if (quad.bHasCullFace)
        {
            if (_ShouldCullFace(world, wx, wy, wz, static_cast<FACE_DIR>(quad.cullFaceDir)))
                continue;
        }

        _AppendQuad(quad, lx, ly, lz, outMesh);
    }

    return true;
}

bool CChunkMeshBuilder::_ShouldCullFace(const CChunkWorld& world, int wx, int wy, int wz, FACE_DIR dir) const
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

    return world.IsSolid(world.GetBlock(wx + n.x, wy + n.y, wz + n.z));
}

bool CChunkMeshBuilder::_AppendQuad(const BakedQuad& quad, int lx, int ly, int lz, ChunkMeshData& outMesh) const
{
    AtlasRegion region{};

    // TODO: #1 Change String key to Hash key
    if (!BlockResDB.TryGetRegion(quad.debugTextureKey.c_str(), region))
        return false;

    const uint32_t baseIndex = static_cast<uint32_t>(outMesh.vertices.size());

    for (int i = 0; i < 4; ++i)
    {
        ChunkMeshVertex v{};
        v.position.x = quad.verts[i].pos.x + static_cast<float>(lx);
        v.position.y = quad.verts[i].pos.y + static_cast<float>(ly);
        v.position.z = quad.verts[i].pos.z + static_cast<float>(lz);

        v.normal = quad.verts[i].normal;
        v.color = {1.f, 1.f, 1.f, 1.f};
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

XMFLOAT2 CChunkMeshBuilder::_RemapAtlasUV(const AtlasRegion& region, const XMFLOAT2& localUV) const
{
    XMFLOAT2 outUV;
    outUV.x = region.u0 + (region.u1 - region.u0) * localUV.x;
    outUV.y = region.v0 + (region.v1 - region.v0) * localUV.y;
    return outUV;
}

