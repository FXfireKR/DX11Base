#include "pch.h"
#include "CChunkMesherSystem.h"
#include "CScene.h"
#include "CObject.h"
#include "CChunkMeshBuilder.h"
#include "CRenderWorld.h"
#include "CMeshRenderer.h"
#include "CMeshManager.h"
#include "CChunkWorld.h"

static uint64_t MakeChunkMeshKey(int cx, int cy, int cz)
{
	// packing
	uint64_t x = (uint32_t)(cx & 0xFFFF);
	uint64_t y = (uint32_t)(cy & 0xFFFF);
	uint64_t z = (uint32_t)(cz & 0xFFFF);
	return (x) | (y << 16) | (z << 32);
}

void CChunkMesherSystem::RebuildDirtyChunks(CScene& scene, CChunkWorld& world)
{
	CChunkMeshBuilder builder;

	SectionCoord sectionCoord{};
	while (world.PopDirty(sectionCoord))
	{
		CChunkSection* pSection = world.FindSectionDataMutable(sectionCoord.x, sectionCoord.y, sectionCoord.z);
		if (nullptr == pSection) 
			continue;

		pSection->SetBuildQueued(false);
		if (!pSection->IsDirty())
			continue;

		ChunkMeshData meshData;
		builder.BuildSectionMesh(world, sectionCoord.x, sectionCoord.y, sectionCoord.z, *pSection, meshData);

		CObject* pRenderObject = world.FindRenderObject(sectionCoord.x, sectionCoord.y, sectionCoord.z);
		if (nullptr == pRenderObject)
			continue;

		auto* meshRender = pRenderObject->GetComponent<CMeshRenderer>();
		if (nullptr == meshRender)
			continue;

		const uint64_t meshKey = MakeChunkMeshKey(sectionCoord.x, sectionCoord.y, sectionCoord.z);
		CMesh* mesh = scene.GetRenderWorld().GetMeshManager().CreateOrUpdateDynamicMesh(scene.GetRenderWorld().GetContext()
			, meshKey, meshData.vertices.data(), sizeof(ChunkMeshVertex), meshData.vertices.size(), meshData.indices.data(), meshData.indices.size());

		meshRender->SetMesh(mesh);
		pSection->ClearDirty();
	}
}

/*
ChunkCoord sc{};
	while (world.PopDirty(sc))
	{
		ChunkSection* sec = world.FindSectionDataMutable(sc.x, sc.y, sc.z);
		if (!sec) continue;

		sec->dirtyQueued = false;
		if (!sec->dirty) continue;

		ChunkMeshData meshData;
		CChunkMesher::BuildFromBakedModels(world, scene.GetRenderWorld().GetRuntimeAtlas(), sc.x, sc.y, sc.z, *sec, meshData);

		CObject* ro = world.FindRenderObject(sc.x, sc.y, sc.z);
		if (!ro) continue;

		auto* mr = ro->GetComponent<CMeshRenderer>();
		if (!mr) continue;

		const uint64_t meshKey = ((uint64_t)(uint32_t)sc.x) ^ (((uint64_t)(uint32_t)sc.y) << 32) ^ (((uint64_t)sc.z) << 48);

		CMesh* mesh = scene.GetRenderWorld().GetMeshManager().CreateOrUpdateDynamicMesh(scene.GetRenderWorld().GetContext(), meshKey,
			meshData.vertices.data(), sizeof(CHUNK_VERTEX), meshData.vertices.size(),
			meshData.indices.data(), meshData.indices.size());

		mr->SetMesh(mesh);
		sec->dirty = false;
	}

*/

//void CChunkMesherSystem::RebuildDirtyChunks(CScene& scene)
//{
//	// Scene이 가진 모든 오브젝트 순회 or COMPONENT-POOL에서 돌리기
//	scene.GetObjectManager().ForEachAliveEnabled([&](CObject& obj) {
//		auto* chunk = obj.GetComponent<CChunkComponent>();
//		if (!chunk) return;
//
//		if (chunk->IsDirty()) {
//			CMeshRenderer* meshRender = chunk->GetMeshRenderer();
//			if (!meshRender) return;
//
//			ChunkMeshData meshData;
//			CChunkMesher::BuildNaive(scene.GetRenderWorld().GetRuntimeAtlas(), *chunk, meshData);
//
//			const uint64_t key = MakeChunkMeshKey(chunk->GetChunkCoord());
//			CMesh* mesh = scene.GetRenderWorld().GetMeshManager().CreateOrUpdateDynamicMesh(scene.GetRenderWorld().GetContext(), key,
//				meshData.vertices.data(), sizeof(CHUNK_VERTEX), meshData.vertices.size(),
//				meshData.indices.data(), meshData.indices.size());
//
//			chunk->SetMesh(mesh);
//			meshRender->SetMesh(mesh);
//			chunk->ClearDirty();
//		}
//		});
//}