#include "pch.h"
#include "CChunkMesherSystem.h"
#include "CScene.h"
#include "CObject.h"
#include "CChunkComponent.h"
#include "CChunkMesher.h"
#include "CMeshRenderer.h"
#include "CMeshManager.h"

static uint64_t MakeChunkMeshKey(const ChunkCoord& c)
{
	// packing
	uint64_t x = (uint32_t)(c.x & 0xFFFF);
	uint64_t y = (uint32_t)(c.y & 0xFFFF);
	uint64_t z = (uint32_t)(c.z & 0xFFFF);
	return (x) | (y << 16) | (z << 32);
}

void CChunkMesherSystem::RebuildDirtyChunks(CScene& scene)
{
	// Scene이 가진 모든 오브젝트 순회 or COMPONENT-POOL에서 돌리기
	
}
