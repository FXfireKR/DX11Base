#pragma once

using CHUNK_VERTEX = VERTEX_POSITION_UV_NORMAL;

struct ChunkMeshData
{
	vector<CHUNK_VERTEX> vertices;
	vector<uint32_t> indices;
};

class CChunkComponent;
class CRuntimeAtlas;

class CChunkMesher
{
public:
	static void BuildNaive(const CRuntimeAtlas& atlas, const CChunkComponent& chunk, ChunkMeshData& out);
};