#include "pch.h"
#include "CChunkMesher.h"
#include "CChunkWorld.h"
#include "CRuntimeAtlas.h"
#include "CChunkComponent.h"
#include "ChunkMath.h"

using namespace ChunkMath;

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

static bool IsAirWorld(const CChunkWorld& world, int x, int y, int z)
{
	return world.GetBlock(x, y, z);
}

static bool IsAirNeighbor(const CChunkWorld& world, const ChunkSection& section, int cx, int sy, int cz, int x, int y, int z)
{
	if (x >= 0 && x < CHUNK_SIZE_X &&
		y >= 0 && y < CHUNK_SECTION_SIZE &&
		z >= 0 && z < CHUNK_SIZE_Z)
	{
		return section.blocks[IndexSection(x, y, z)].blockID == 0;
	}

	const int wx = cx * CHUNK_SIZE_X + x;
	const int wy = sy * CHUNK_SECTION_SIZE + y;
	const int wz = cz * CHUNK_SIZE_Z + z;

	return world.GetBlock(wx, wy, wz) == 0;
}

static bool IsFaceOccluded(const CChunkWorld& world, int wx, int wy, int wz, uint8_t cullDir)
{
	switch (cullDir)
	{
		case static_cast<uint8_t>(FACE_DIR::PX): ++wx; break;
		case static_cast<uint8_t>(FACE_DIR::NX): --wx; break;
		case static_cast<uint8_t>(FACE_DIR::PY): ++wy; break;
		case static_cast<uint8_t>(FACE_DIR::NY): --wy; break;
		case static_cast<uint8_t>(FACE_DIR::PZ): ++wz; break;
		case static_cast<uint8_t>(FACE_DIR::NZ): --wz; break;
		default: return false;
	}

	return world.GetBlock(wx, wy, wz) != 0;
}

void CChunkMesher::AppendBakedQuad(IN const BakedQuad& quad, const CRuntimeAtlas& atlas, const XMFLOAT3& blockOffset, OUT vector<CHUNK_VERTEX>& verts, vector<uint32_t>& indices)
{
	/*const uint16_t titleID = atlas.GetTileID(quad.textureHash);
	const UVRect uv = (titleID == UINT16_MAX) ? atlas.GetUV(0) : atlas.GetUV(titleID);
	const uint32_t baseIndex = static_cast<uint32_t>(verts.size());

	for (int i = 0; i < 4; ++i)
	{
		const BakedVertex& bakedVert = quad.verts[i];

		XMFLOAT3 pos
		{
			bakedVert.pos.x + blockOffset.x,
			bakedVert.pos.y + blockOffset.y,
			bakedVert.pos.z + blockOffset.z
		};

		XMFLOAT2 uvAtlas
		{
			uv.u0 + (uv.u1 - uv.u0) * bakedVert.uv.x,
			uv.v0 + (uv.v1 - uv.v0) * bakedVert.uv.y,
		};

		verts.push_back({ {pos}, {uvAtlas}, {bakedVert.normal} });
	}

	indices.push_back(baseIndex + 0);
	indices.push_back(baseIndex + 1);
	indices.push_back(baseIndex + 2);

	indices.push_back(baseIndex + 0);
	indices.push_back(baseIndex + 2);
	indices.push_back(baseIndex + 3);*/
}

void CChunkMesher::BuildFromBakedModels(IN const CChunkWorld& world, const CRuntimeAtlas& atlas, int cx, int sy, int cz, const ChunkSection& section, OUT ChunkMeshData& meshData)
{
	meshData.vertices.clear();
	meshData.indices.clear();

	for (int z = 0; z < CHUNK_SECTION_SIZE; ++z)
	{
		for (int y = 0; y < CHUNK_SECTION_SIZE; ++y)
		{
			for (int x = 0; x < CHUNK_SECTION_SIZE; ++x)
			{
				const BlockCell& cell = section.blocks[IndexSection(x, y, z)];
				if (cell.blockID == 0) continue;

				vector<AppliedModel> vecAppliedModels;
				BlockDB.GetAppliedModels(cell.blockID, cell.stateIndex, vecAppliedModels);

				const int wx = cx * CHUNK_SECTION_SIZE + x;
				const int wy = sy * CHUNK_SECTION_SIZE + y;
				const int wz = cz * CHUNK_SECTION_SIZE + z;

				const XMFLOAT3 blockOffset((float)x, (float)y, (float)z);

				for (const AppliedModel& applied : vecAppliedModels)
				{
					const BakedModel* bakedModel = BlockDB.FindBakedModel(applied.modelHash);
					if (!bakedModel) continue;

					for (const BakedQuad& quad : bakedModel->quads)
					{
						if (quad.bHasCullFace)
						{
							if (IsFaceOccluded(world, wx, wy, wz, quad.cullFaceDir)) continue;
						}

						AppendBakedQuad(quad, atlas, blockOffset, meshData.vertices, meshData.indices);
					}
				}
			}
		}
	}
}

//void CChunkMesher::BuildNaive(IN const CChunkWorld& world, const CRuntimeAtlas& atlas, int cx, int sy, int cz, const ChunkSection& section, OUT ChunkMeshData& meshData)
//{
//	const int baseX = cx * CHUNK_SIZE_X;
//	const int baseY = sy * CHUNK_SECTION_SIZE;
//	const int baseZ = cz * CHUNK_SIZE_Z;
//
//	for (int z = 0; z < CHUNK_SIZE_Z; ++z) 
//	{
//		for (int y = 0; y < CHUNK_SECTION_SIZE; ++y)
//		{
//			for (int x = 0; x < CHUNK_SIZE_X; ++x)
//			{
//				const BLOCK_ID id = section.blocks[Index16(x, y, z)];
//				if (id == 0) continue;
//
//				UVRect uv = atlas.GetUV(id - 1);
//
//				const int wx = baseX + x;
//				const int wy = baseY + y;
//				const int wz = baseZ + z;
//
//				if (IsAirNeighbor(world, section, cx, sy, cz, x + 1, y, z));
//					AddFaceQuad(x, y, z, FACE_DIR::PX, uv, meshData.vertices, meshData.indices); // +x
//
//				if (IsAirNeighbor(world, section, cx, sy, cz, x - 1, y, z));
//					AddFaceQuad(x, y, z, FACE_DIR::NX, uv, meshData.vertices, meshData.indices); // -x
//
//					if (IsAirNeighbor(world, section, cx, sy, cz, x, y + 1, z));
//					AddFaceQuad(x, y, z, FACE_DIR::PY, uv, meshData.vertices, meshData.indices); // +y
//
//					if (IsAirNeighbor(world, section, cx, sy, cz, x, y - 1, z));
//					AddFaceQuad(x, y, z, FACE_DIR::NY, uv, meshData.vertices, meshData.indices); // -y
//
//					if (IsAirNeighbor(world, section, cx, sy, cz, x, y, z + 1));
//					AddFaceQuad(x, y, z, FACE_DIR::PZ, uv, meshData.vertices, meshData.indices); // +z
//
//					if (IsAirNeighbor(world, section, cx, sy, cz, x, y, z - 1));
//					AddFaceQuad(x, y, z, FACE_DIR::NZ, uv, meshData.vertices, meshData.indices); // -z
//			}
//		}
//	}
//}

//void CChunkMesher::BuildNaive(const CRuntimeAtlas& atlas, const CChunkComponent& chunk, ChunkMeshData& out)
//{
//	out.vertices.clear();
//	out.indices.clear();
//
//	for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
//		for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
//			for (int x = 0; x < CHUNK_SIZE_X; ++x)
//			{
//				const BLOCK_ID id = chunk.GetBlock(x, y, z);
//				if (id == 0) continue;
//
//				UVRect uv = atlas.GetUV(id - 1);
//
//				if (IsAir(chunk, x + 1, y, z)) 
//					AddFaceQuad(x, y, z, FACE_DIR::PX, uv, out.vertices, out.indices); // +x
//
//				if (IsAir(chunk, x - 1, y, z)) 
//					AddFaceQuad(x, y, z, FACE_DIR::NX, uv, out.vertices, out.indices); // -x
//
//				if (IsAir(chunk, x, y + 1, z)) 
//					AddFaceQuad(x, y, z, FACE_DIR::PY, uv, out.vertices, out.indices); // +y
//
//				if (IsAir(chunk, x, y - 1, z)) 
//					AddFaceQuad(x, y, z, FACE_DIR::NY, uv, out.vertices, out.indices); // -y
//
//				if (IsAir(chunk, x, y, z + 1)) 
//					AddFaceQuad(x, y, z, FACE_DIR::PZ, uv, out.vertices, out.indices); // +z
//
//				if (IsAir(chunk, x, y, z - 1)) 
//					AddFaceQuad(x, y, z, FACE_DIR::NZ, uv, out.vertices, out.indices); // -z
//			}
//		}
//	}
//}
