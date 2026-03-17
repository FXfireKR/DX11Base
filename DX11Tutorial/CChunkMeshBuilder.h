#pragma once
#include "ChunkTypes.h"

class CChunkWorld;
class CChunkColumn;
class CChunkSection;

class CChunkMeshBuilder
{
public:
	bool BuildSectionMesh(const CChunkWorld& world, int cx, int sy, int cz
		, const CChunkSection& section, ChunkMeshData& outMesh) const;

private:
	bool _AppendBlockQuads(const CChunkWorld& world, int wx, int wy, int wz
		, const BlockCell& cell, ChunkMeshData& outMesh) const;

	bool _ShouldCullFace(const CChunkWorld& world, int wx, int wy, int wz
		, FACE_DIR dir) const;

	bool _AppendQuad(const BakedQuad& quad, int wx, int wy, int wz
		, ChunkMeshData& outMesh) const;

	XMFLOAT2 _RemapAtlasUV(const AtlasRegion& region, const XMFLOAT2& localUV) const;
};

/*
public:
	bool BuildSectionMesh(const CChunkWorld& world, const CChunkColumn& column, int sectionY
		, const CChunkSection& section, ChunkMeshData& outMesh) const;

private:
	bool _AppendBlockQuads(const CChunkWorld& world, int wx, int wy, int wz, const BlockCell& cell, ChunkMeshData& outMesh) const;
	bool _AppendQuad(const BakedQuad& quad, int wx, int wy, int wz, ChunkMeshData& outMesh) const;
	XMFLOAT2 _RemapAtlasUV(const AtlasRegion& region, const XMFLOAT2& localUV) const;

*/

//bool CChunkMeshBuilder::BuildSectionMesh(const CChunkWorld& world, const CChunkColumn& column, int sectionY, const CChunkSection& section, ChunkMeshData& outMesh) const
//{
//    outMesh.Clear();
//
//    const int chunkX = column.GetChunkX();
//    const int chunkZ = column.GetChunkZ();
//
//    int baseWx = chunkX * CHUNK_SIZE_X;
//    int baseWy = sectionY * CHUNK_SECTION_SIZE;
//    int baseWz = chunkZ * CHUNK_SIZE_Z;
//
//    for (int ly = 0; ly < CHUNK_SECTION_SIZE; ++ly)
//    {
//        for (int lz = 0; lz < CHUNK_SECTION_SIZE; ++lz)
//        {
//            for (int lx = 0; lx < CHUNK_SECTION_SIZE; ++lx)
//            {
//                const BlockCell cell = section.GetLocalCell(lx, ly, lz);
//                if (cell.blockID == 0)
//                    continue;
//
//                const int wx = baseWx + lx;
//                const int wy = baseWy + ly;
//                const int wz = baseWz + lz;
//
//                bool result = _AppendBlockQuads(world, wx, wy, wz, cell, outMesh);
//                if (!result)
//                {
//                    // some...
//                }
//            }
//        }
//    }
//
//    return true;
//}