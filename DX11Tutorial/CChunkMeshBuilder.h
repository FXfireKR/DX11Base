#pragma once
#include "ChunkTypes.h"

class CChunkWorld;
class CChunkColumn;
class CChunkSection;

class CChunkMeshBuilder
{
public:
	bool BuildSectionMeshes(const CChunkWorld& world, int cx, int sy, int cz
		, const CChunkSection& section, ChunkSectionMeshSet& outMeshes) const;

private:
	bool _AppendBlockQuads(const CChunkWorld& world
		, int wx, int wy, int wz, int lx, int ly, int lz
		, const BlockCell& cell, ChunkSectionMeshSet& outMeshes) const;

	bool _ShouldCullFace(const CChunkWorld& world, int wx, int wy, int wz, const BlockCell& cell
		, FACE_DIR dir) const;

	bool _AppendQuad(const CChunkWorld& world, const BakedQuad& quad
		, int wx, int wy, int wz, int lx, int ly, int lz
		, ChunkMeshData& outMesh) const;

	uint8_t _ResolveQuadBlockLight(const CChunkWorld& world, const BakedQuad& quad
		, int wx, int wy, int wz) const;

	XMFLOAT4 _ResolveQuadColor_DebugBlockLight(const CChunkWorld& world, const BakedQuad& quad
		, int wx, int wy, int wz) const;

	XMFLOAT4 ResolveBlockTint(const BakedQuad& quad) const;
	XMFLOAT2 _RemapAtlasUV(const AtlasRegion& region, const XMFLOAT2& localUV) const;
};