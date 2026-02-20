#include "pch.h"
#include "CChunkMesherSystem.h"
#include "CScene.h"
#include "CObject.h"
#include "CChunkComponent.h"
#include "CChunkMesher.h"
#include "CRenderWorld.h"
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
	scene.GetObjectManager().ForEachAliveEnabled([&](CObject& obj) {
		auto* chunk = obj.GetComponent<CChunkComponent>();
		if (!chunk) return;

		if (chunk->IsDirty()) {
			CMeshRenderer* meshRender = chunk->GetMeshRenderer();
			if (!meshRender) return;

			ChunkMeshData meshData;
			CChunkMesher::BuildNaive(scene.GetRenderWorld().GetRuntimeAtlas(), *chunk, meshData);

			const uint64_t key = MakeChunkMeshKey(chunk->GetChunkCoord());
			CMesh* mesh = scene.GetRenderWorld().GetMeshManager().CreateOrUpdateDynamicMesh(scene.GetRenderWorld().GetContext(), key,
				meshData.vertices.data(), sizeof(CHUNK_VERTEX), meshData.vertices.size(),
				meshData.indices.data(), meshData.indices.size());

			chunk->SetMesh(mesh);
			meshRender->SetMesh(mesh);
			chunk->ClearDirty();
		}
	});
}
