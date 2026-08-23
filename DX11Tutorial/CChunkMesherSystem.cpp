#include "pch.h"
#include "CChunkMesherSystem.h"
#include "CScene.h"
#include "CObject.h"
#include "CChunkMeshBuilder.h"
#include "CRenderWorld.h"
#include "CMeshRenderer.h"
#include "CMeshManager.h"
#include "CChunkWorld.h"

static uint64_t MakeChunkMeshKey(int cx, int cy, int cz, EChunkSectionRenderSlot slot)
{
	// packing
	uint64_t x = (uint32_t)(cx & 0xFFFF);
	uint64_t y = (uint32_t)(cy & 0xFFFF);
	uint64_t z = (uint32_t)(cz & 0xFFFF);
	uint64_t s = (uint64_t)(static_cast<uint8_t>(slot) & 0xFF);


	return (x) | (y << 16) | (z << 32) | (s << 48);
}

void CChunkMesherSystem::RebuildDirtyChunks(CScene& scene, CChunkWorld& world)
{
	PROFILE_SCOPE();

	float totalBuildMs = 0.f;
	float totalUploadMs = 0.f;

	CChunkMeshBuilder builder;
	SectionCoord sectionCoord{};

#ifdef OPTIMIZATION_CHUNK_BUILD_SYSTEM
	constexpr int MIN_SECTION_PER_FRAME = 1;
	constexpr int MAX_SECTION_PER_FRAME = 4;

#ifdef _DEBUG
	constexpr float MESH_TIME_BUDGET_MS = 5.0f;
#else // _DEBUG
	constexpr float MESH_TIME_BUDGET_MS = 2.0f;
#endif // _DEBUG

	LARGE_INTEGER freq{};
	LARGE_INTEGER begin{};

	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&begin);

	int iRebuiltCount = 0;

	float fTotalBuildMs = 0.f;
	float fTotalUploadMs = 0.f;

	while (iRebuiltCount < MAX_SECTION_PER_FRAME && world.PopDirty(sectionCoord))
	{
		if (iRebuiltCount >= MIN_SECTION_PER_FRAME)
		{
			LARGE_INTEGER now{};
			QueryPerformanceCounter(&now);

			const double elapsedMs = static_cast<double>(now.QuadPart - begin.QuadPart) * 1000.0 / static_cast<double>(freq.QuadPart);

			if (elapsedMs >= MESH_TIME_BUDGET_MS)
				break;
		}

		CChunkSection* pSection = world.FindSectionDataMutable(sectionCoord.x, sectionCoord.y, sectionCoord.z);
		if (nullptr == pSection)
			continue;

#ifdef OPTIMIZATION_2
		const bool bMeshDirty = pSection->IsMeshDirty();
		const bool bLightDirty = pSection->IsLightDirty();
#endif // OPTIMIZATION_2

		pSection->SetBuildQueued(false);

#ifdef OPTIMIZATION_2
		if (!bMeshDirty && !bLightDirty)
			continue;
#else // OPTIMIZATION_2
		if (!pSection->IsDirty())
			continue;
#endif // OPTIMIZATION_2

		ChunkSectionMeshSet meshSet;

		float buildMs = 0.f;
		{
			CScopedCpuTimer timer(buildMs);

			builder.BuildSectionMeshes(
				world,
				sectionCoord.x,
				sectionCoord.y,
				sectionCoord.z,
				*pSection,
				meshSet);
		}

		totalBuildMs += buildMs;

		float uploadMs = 0.f;
		{
			CScopedCpuTimer timer(uploadMs);

			UploadSectionMesh(
				scene,
				world,
				sectionCoord,
				EChunkSectionRenderSlot::OPAQUE_SLOT,
				meshSet.opaque);

			UploadSectionMesh(
				scene,
				world,
				sectionCoord,
				EChunkSectionRenderSlot::CUTOUT_SLOT,
				meshSet.cutout);

			UploadSectionMesh(
				scene,
				world,
				sectionCoord,
				EChunkSectionRenderSlot::TRANSLUCENT_SLOT,
				meshSet.translucent);
		}

		totalUploadMs += uploadMs;

#ifdef OPTIMIZATION_2
		if (bMeshDirty)
			pSection->ClearDirty();

		if (bMeshDirty || bLightDirty)
			pSection->ClearLightDirty();
#else // OPTIMIZATION_2
		pSection->ClearDirty();
#endif // OPTIMIZATION_2

		dbg.AddRebuiltSection();
		++iRebuiltCount;
	}

#else // OPTIMIZATION_CHUNK_BUILD_SYSTEM
	constexpr int MIN_BUDGET = 4;
	//int iCurrentBudget = std::max(MIN_BUDGET, static_cast<int>(world.GetDirtyQueueSize() / 100));

	int iCurrentBudget = MIN_BUDGET;

	while (iCurrentBudget > 0 && world.PopDirty(sectionCoord))
	{
		CChunkSection* pSection = world.FindSectionDataMutable(sectionCoord.x, sectionCoord.y, sectionCoord.z);
		if (nullptr == pSection) 
			continue;

#ifdef OPTIMIZATION_2
		const bool bMeshDirty = pSection->IsMeshDirty();
		const bool bLightDirty = pSection->IsLightDirty();
#endif // OPTIMIZATION_2

		pSection->SetBuildQueued(false);

#ifdef OPTIMIZATION_2
		if (!bMeshDirty && !bLightDirty)
			continue;
#else // OPTIMIZATION_2
		if (!pSection->IsDirty())
			continue;
#endif // OPTIMIZATION_2

		ChunkSectionMeshSet meshSet;

		float buildMs = 0.f;
		{
			CScopedCpuTimer timer(buildMs);

			builder.BuildSectionMeshes(
				world,
				sectionCoord.x,
				sectionCoord.y,
				sectionCoord.z,
				*pSection,
				meshSet);
		}

		totalBuildMs += buildMs;

		float uploadMs = 0.f;
		{
			CScopedCpuTimer timer(uploadMs);

			UploadSectionMesh(
				scene,
				world,
				sectionCoord,
				EChunkSectionRenderSlot::OPAQUE_SLOT,
				meshSet.opaque);

			UploadSectionMesh(
				scene,
				world,
				sectionCoord,
				EChunkSectionRenderSlot::CUTOUT_SLOT,
				meshSet.cutout);

			UploadSectionMesh(
				scene,
				world,
				sectionCoord,
				EChunkSectionRenderSlot::TRANSLUCENT_SLOT,
				meshSet.translucent);
		}

		totalUploadMs += uploadMs;

#ifdef OPTIMIZATION_2
		if (bMeshDirty)
			pSection->ClearDirty();

		if (bMeshDirty || bLightDirty)
			pSection->ClearLightDirty();
#else // OPTIMIZATION_2
		pSection->ClearDirty();
#endif // OPTIMIZATION_2

		dbg.AddRebuiltSection();
		--iCurrentBudget;
	}
#endif // OPTIMIZATION_CHUNK_BUILD_SYSTEM

	dbg.SetMeshBuildMs(totalBuildMs);
	dbg.SetMeshUploadMs(totalUploadMs);
}

void CChunkMesherSystem::UploadSectionMesh(CScene& scene, CChunkWorld& world, const SectionCoord& sectionCoord
	, EChunkSectionRenderSlot slot, const ChunkMeshData& meshData)
{
	PROFILE_SCOPE();

	CObject* pRenderObject = world.FindRenderObject(sectionCoord.x, sectionCoord.y, sectionCoord.z, slot);
	if (nullptr == pRenderObject)
		return;

	auto* meshRender = pRenderObject->GetComponent<CMeshRenderer>();
	if (nullptr == meshRender)
		return;

	const uint64_t meshKey = MakeChunkMeshKey(sectionCoord.x, sectionCoord.y, sectionCoord.z, slot);
	
	CMesh* mesh = scene.GetRenderWorld().GetMeshManager().CreateOrUpdateDynamicMesh(
		scene.GetRenderWorld().GetContext(),
		meshKey,
		meshData.vertices.data(),
		sizeof(ChunkMeshVertex),
		static_cast<uint32_t>(meshData.vertices.size()),
		meshData.indices.data(),
		static_cast<uint32_t>(meshData.indices.size())
	);

	if (nullptr == mesh)
		return;

	meshRender->SetMesh(mesh);
	pRenderObject->SetEnable(!meshData.Empty());
}
