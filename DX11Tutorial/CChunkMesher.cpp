#include "pch.h"
#include "CChunkMesher.h"
#include "CChunkComponent.h"

// Positive(+), Negative(-)
enum FaceDir {PX, NX, PY, NY, PZ, NZ};

static XMFLOAT3 FaceNormal(FaceDir eDir)
{
	switch (eDir)
	{
		case PX: return { 1.f, 0.f, 0.f };
		case NX: return { -1.f, 0.f, 0.f };
		case PY: return { 0.f, 1.f, 0.f };
		case NY: return { 0.f, -1.f, 0.f };
		case PZ: return { 0.f, 0.f, 1.f };
		case NZ: return { 0.f, 0.f, -1.f };
	}
}

static void AddFaceQuad(int x, int y, int z, FaceDir eDir, vector<CHUNK_VERTEX>& v, vector<uint32_t>& i)
{
	const XMFLOAT3 n = FaceNormal(eDir);
	XMFLOAT3 p[4];

	switch (eDir)
	{
		case PX: // x+ 면
		{
			p[0] = { (float)(x + 1), (float)y,     (float)z };
			p[1] = { (float)(x + 1), (float)(y + 1), (float)z };
			p[2] = { (float)(x + 1), (float)(y + 1), (float)(z + 1) };
			p[3] = { (float)(x + 1), (float)y,     (float)(z + 1) };
		} break;

		case NX: // x- 면
		{
			p[0] = { (float)x, (float)y,     (float)(z + 1) };
			p[1] = { (float)x, (float)(y + 1), (float)(z + 1) };
			p[2] = { (float)x, (float)(y + 1), (float)z };
			p[3] = { (float)x, (float)y,     (float)z };
		} break;

		case PY: // y+ 면
		{
			p[0] = { (float)x,     (float)(y + 1), (float)z };
			p[1] = { (float)x,     (float)(y + 1), (float)(z + 1) };
			p[2] = { (float)(x + 1), (float)(y + 1), (float)(z + 1) };
			p[3] = { (float)(x + 1), (float)(y + 1), (float)z };
		} break;

		case NY: // y- 면
		{
			p[0] = { (float)x,     (float)y, (float)(z + 1) };
			p[1] = { (float)x,     (float)y, (float)z };
			p[2] = { (float)(x + 1), (float)y, (float)z };
			p[3] = { (float)(x + 1), (float)y, (float)(z + 1) };
		} break;

		case PZ: // z+ 면
		{
			p[0] = { (float)(x + 1), (float)y,     (float)(z + 1) };
			p[1] = { (float)(x + 1), (float)(y + 1), (float)(z + 1) };
			p[2] = { (float)x,     (float)(y + 1), (float)(z + 1) };
			p[3] = { (float)x,     (float)y,     (float)(z + 1) };
		} break;

		case NZ: // z- 면
		default:
		{
			p[0] = { (float)x,     (float)y,     (float)z };
			p[1] = { (float)x,     (float)(y + 1), (float)z };
			p[2] = { (float)(x + 1), (float)(y + 1), (float)z };
			p[3] = { (float)(x + 1), (float)y,     (float)z };
		} break;
	}

	const uint32_t base = (uint32_t)v.size();

	// TODO: UV좌표는 나중에 atlas 방식으로 하면 변경해야함.
	v.push_back({ p[0], {0,1}, n });
	v.push_back({ p[1], {0,0}, n });
	v.push_back({ p[2], {1,0}, n });
	v.push_back({ p[3], {1,1}, n });

	i.push_back(base + 0);
	i.push_back(base + 1);
	i.push_back(base + 2);
	i.push_back(base + 0);
	i.push_back(base + 3);
	i.push_back(base + 3);
}

static bool IsAir(const CChunkComponent& c, int x, int y, int z)
{
	return c.GetBlock(x, y, z) == 0;
}

void CChunkMesher::BuildNaive(__in const CChunkComponent& chunk, __out ChunkMeshData& out)
{
	out.vertices.clear();
	out.indices.clear();

	for (int z = 0; z < CHUNK_SIZE_Z; ++z)
	for (int y = 0; y < CHUNK_SIZE_Y; ++y)
	for (int x = 0; x < CHUNK_SIZE_X; ++x)
	{
		const BLOCK_ID id = chunk.GetBlock(x, y, z);
		if (id == 0) continue;

		if (IsAir(chunk, x + 1, y, z)) AddFaceQuad(x, y, z, PX, out.vertices, out.indices);
		if (IsAir(chunk, x - 1, y, z)) AddFaceQuad(x, y, z, NX, out.vertices, out.indices);
		if (IsAir(chunk, x, y + 1, z)) AddFaceQuad(x, y, z, PY, out.vertices, out.indices);
		if (IsAir(chunk, x, y - 1, z)) AddFaceQuad(x, y, z, NY, out.vertices, out.indices);
		if (IsAir(chunk, x, y, z + 1)) AddFaceQuad(x, y, z, PZ, out.vertices, out.indices);
		if (IsAir(chunk, x, y, z - 1)) AddFaceQuad(x, y, z, NZ, out.vertices, out.indices);
	}
}
