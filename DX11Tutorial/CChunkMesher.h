#pragma once
#include "VertexTypes.h"
#include "VertexLayoutTypes.h"
#include "ChunkTypes.h"

using CHUNK_VERTEX = VERTEX_POSITION_UV_NORMAL;

struct ChunkMeshData
{
	vector<CHUNK_VERTEX> vertices;
	vector<uint32_t> indices;
};

class CChunkComponent;
class CRuntimeAtlas;
class CChunkWorld;

class CChunkMesher
{
public:
	static void AppendBakedQuad(IN const BakedQuad& quad, const CRuntimeAtlas& atlas, const XMFLOAT3& blockOffset,
		OUT vector<CHUNK_VERTEX>& verts, vector<uint32_t>& indices);

	static void BuildFromBakedModels(IN const CChunkWorld& world,
		const CRuntimeAtlas& atlas,
		int cx, int sy, int cz,
		const ChunkSection& section,
		OUT ChunkMeshData& meshData);

	/*static void BuildNaive(IN const CChunkWorld& world, 
		const CRuntimeAtlas& atlas,
		int cx, int sy, int cz, 
		const ChunkSection& section, 
		OUT ChunkMeshData& meshData);*/

	//static void BuildNaive(const CRuntimeAtlas& atlas, const CChunkComponent& chunk, ChunkMeshData& out);
};