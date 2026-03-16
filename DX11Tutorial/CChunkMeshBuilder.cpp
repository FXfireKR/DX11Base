#include "pch.h"
#include "CChunkMeshBuilder.h"
#include "CChunkWorld.h"

bool CChunkMeshBuilder::BuildSectionMesh(const CChunkWorld& world, const CChunkColumn& column, int sectionY, const CChunkSection& section, ChunkMeshData& outMesh) const
{
    outMesh.Clear();

    const int chunkX = column.GetChunkX();
    const int chunkZ = column.GetChunkZ();

    int baseWx = chunkX * CHUNK_SIZE_X;
    int baseWy = sectionY * CHUNK_SECTION_SIZE;
    int baseWz = chunkZ * CHUNK_SIZE_Z;

    for (int ly = 0; ly < CHUNK_SECTION_SIZE; ++ly)
    {
        for (int lz = 0; lz < CHUNK_SECTION_SIZE; ++lz)
        {
            for (int lx = 0; lx < CHUNK_SECTION_SIZE; ++lx)
            {
                const BlockCell cell = section.GetLocalCell(lx, ly, lz);
                if (cell.blockID == 0)
                    continue;

                const int wx = baseWx + lx;
                const int wy = baseWy + ly;
                const int wz = baseWz + lz;

                bool result = _AppendBlockQuads(world, wx, wy, wz, cell, outMesh);
                if (!result)
                {
                    // some...
                }
            }
        }
    }

    return true;
}

bool CChunkMeshBuilder::_AppendBlockQuads(const CChunkWorld& world, int wx, int wy, int wz, const BlockCell& cell, ChunkMeshData& outMesh) const
{
    const BakedModel* pBakedModel = BlockDB.GetBakedModel(cell.blockID, cell.stateIndex);
    if (nullptr == pBakedModel)
        return false;

    for (const BakedQuad& quad : pBakedModel->quads)
    {
        if (quad.bHasCullFace)
        {
            XMINT3 n{};
            FACE_DIR eDir = static_cast<FACE_DIR>(quad.cullFaceDir);
            switch (eDir)
            {
                case FACE_DIR::PX: n = { 0, -1, 0 }; break;
                case FACE_DIR::NX: n = { 0, 1, 0 }; break;
                case FACE_DIR::PY: n = { 0, 0, -1 }; break;
                case FACE_DIR::NY: n = { 0, 0, 1 }; break;
                case FACE_DIR::PZ: n = { -1, 0, 0 }; break;
                case FACE_DIR::NZ: n = { 1, 0, 0 }; break;
                default: break;
            }

            if (world.IsSolidBlock(wx + n.x, wy + n.y, wz + n.z))
                continue;
        }

        _AppendQuad(quad, wx, wy, wz, outMesh);
    }

    return true;
}

bool CChunkMeshBuilder::_AppendQuad(const BakedQuad& quad, int wx, int wy, int wz, ChunkMeshData& outMesh) const
{
    AtlasRegion region{};

    // TODO: #1 Change String key to Hash key
    if (!BlockResDB.TryGetRegion(quad.debugTextureKey.c_str(), region))
        return false;

    const uint32_t baseIndex = static_cast<uint32_t>(outMesh.vertices.size());

    for (int i = 0; i < 4; ++i)
    {
        ChunkMeshVertex v{};
        v.position.x = quad.verts[i].pos.x + static_cast<float>(wx);
        v.position.y = quad.verts[i].pos.y + static_cast<float>(wy);
        v.position.z = quad.verts[i].pos.z + static_cast<float>(wz);

        v.normal = quad.verts[i].normal;
        //v.color = quad.verts[i].color;
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
    outUV.x = region.v0 + (region.v1 - region.v0) * localUV.y;
    return outUV;
}
