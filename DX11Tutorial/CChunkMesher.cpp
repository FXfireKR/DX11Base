#include "pch.h"
#include "CChunkMesher.h"
#include "CChunkComponent.h"
#include "CRuntimeAtlas.h"

static void AddFaceQuad(int x, int y, int z, FACE_DIR eDir, UVRect uv, vector<CHUNK_VERTEX>& v, vector<uint32_t>& i)
{
	const XMFLOAT3 n = FaceNormal(eDir);
	XMFLOAT3 p[4];

	switch (eDir)
	{
		case FACE_DIR::PX: // x+ 면
		{
			p[0] = { (float)(x + 1), (float)y,     (float)z };
			p[1] = { (float)(x + 1), (float)(y + 1), (float)z };
			p[2] = { (float)(x + 1), (float)(y + 1), (float)(z + 1) };
			p[3] = { (float)(x + 1), (float)y,     (float)(z + 1) };
		} break;

		case FACE_DIR::NX: // x- 면
		{
			p[0] = { (float)x, (float)y,     (float)(z + 1) };
			p[1] = { (float)x, (float)(y + 1), (float)(z + 1) };
			p[2] = { (float)x, (float)(y + 1), (float)z };
			p[3] = { (float)x, (float)y,     (float)z };
		} break;

		case FACE_DIR::PY: // y+ 면
		{
			p[0] = { (float)x,     (float)(y + 1), (float)z };
			p[1] = { (float)x,     (float)(y + 1), (float)(z + 1) };
			p[2] = { (float)(x + 1), (float)(y + 1), (float)(z + 1) };
			p[3] = { (float)(x + 1), (float)(y + 1), (float)z };
		} break;

		case FACE_DIR::NY: // y- 면
		{
			p[0] = { (float)x,     (float)y, (float)(z + 1) };
			p[1] = { (float)x,     (float)y, (float)z };
			p[2] = { (float)(x + 1), (float)y, (float)z };
			p[3] = { (float)(x + 1), (float)y, (float)(z + 1) };
		} break;

		case FACE_DIR::PZ: // z+ 면
		{
			p[0] = { (float)(x + 1), (float)y,     (float)(z + 1) };
			p[1] = { (float)(x + 1), (float)(y + 1), (float)(z + 1) };
			p[2] = { (float)x,     (float)(y + 1), (float)(z + 1) };
			p[3] = { (float)x,     (float)y,     (float)(z + 1) };
		} break;

		case FACE_DIR::NZ: // z- 면
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
	v.push_back({ p[0], {uv.u0,uv.v1}, n });
	v.push_back({ p[1], {uv.u0,uv.v0}, n });
	v.push_back({ p[2], {uv.u1,uv.v0}, n });
	v.push_back({ p[3], {uv.u1,uv.v1}, n });

	i.push_back(base + 0);
	i.push_back(base + 1);
	i.push_back(base + 2);
	i.push_back(base + 0);
	i.push_back(base + 2);
	i.push_back(base + 3);
}

static bool IsAir(const CChunkComponent& c, int x, int y, int z)
{
	return c.GetBlock(x, y, z) == 0;
}

void CChunkMesher::BuildNaive(const CRuntimeAtlas& atlas, const CChunkComponent& chunk, ChunkMeshData& out)
{
	out.vertices.clear();
	out.indices.clear();

	for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
		for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
			for (int x = 0; x < CHUNK_SIZE_X; ++x)
			{
				const BLOCK_ID id = chunk.GetBlock(x, y, z);
				if (id == 0) continue;

				UVRect uv = atlas.GetUV(id - 1);

				if (IsAir(chunk, x + 1, y, z)) 
					AddFaceQuad(x, y, z, FACE_DIR::PX, uv, out.vertices, out.indices); // +x

				if (IsAir(chunk, x - 1, y, z)) 
					AddFaceQuad(x, y, z, FACE_DIR::NX, uv, out.vertices, out.indices); // -x

				if (IsAir(chunk, x, y + 1, z)) 
					AddFaceQuad(x, y, z, FACE_DIR::PY, uv, out.vertices, out.indices); // +y

				if (IsAir(chunk, x, y - 1, z)) 
					AddFaceQuad(x, y, z, FACE_DIR::NY, uv, out.vertices, out.indices); // -y

				if (IsAir(chunk, x, y, z + 1)) 
					AddFaceQuad(x, y, z, FACE_DIR::PZ, uv, out.vertices, out.indices); // +z

				if (IsAir(chunk, x, y, z - 1)) 
					AddFaceQuad(x, y, z, FACE_DIR::NZ, uv, out.vertices, out.indices); // -z
			}
		}
	}
}
