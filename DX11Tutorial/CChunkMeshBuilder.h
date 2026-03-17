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
	bool _AppendBlockQuads(const CChunkWorld& world
		, int wx, int wy, int wz, int lx, int ly, int lz
		, const BlockCell& cell, ChunkMeshData& outMesh) const;

	bool _ShouldCullFace(const CChunkWorld& world, int wx, int wy, int wz
		, FACE_DIR dir) const;

	bool _AppendQuad(const BakedQuad& quad, int lx, int ly, int lz
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