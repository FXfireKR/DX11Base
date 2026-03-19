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