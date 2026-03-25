#include "pch.h"
#include "CChunkWorld.h"
#include "CScene.h"
#include "CPipeline.h"
#include "CMaterial.h"
#include "CWorld.h"
#include "CFlatChunkGenerator.h"
#include "CHeightmapChunkGenerator.h"

#include "ChunkMath.h"

using namespace ChunkMath;

void CChunkWorld::Initialize(CScene& scene, CPipeline* pOpaquePipeline, CMaterial* pOpaqueMaterial
	, CPipeline* pCutoutPipeline, CMaterial* pCutoutMaterial
	, CPipeline* pTranslucentPipeline, CMaterial* pTranslucentMaterial)
{
	m_pScene = &scene;

	m_pOpaquePipeline = pOpaquePipeline;
	m_pOpaqueMaterial = pOpaqueMaterial;

	m_pCutoutPipeline = pCutoutPipeline;
	m_pCutoutMaterial = pCutoutMaterial;

	m_pTranslucentPipeline = pTranslucentPipeline;
	m_pTranslucentMaterial = pTranslucentMaterial;

	// Chunk generator setting
	{
		m_genSettings.seed = 12345u;
		m_genSettings.baseHeight = 48;
		m_genSettings.heightAmplitude = 12;
		m_genSettings.frequency = 0.01f;
		m_genSettings.seaLevel = 48;
	}

	m_pGenerator = std::make_unique<CHeightmapChunkGenerator>();
	m_pGenerator->Initialize(m_genSettings);
}

void CChunkWorld::UpdateStreaming(const XMFLOAT3& playerWorldPos)
{
	const int centerCx = FloorDiv16((int)std::floor(playerWorldPos.x));
	const int centerCz = FloorDiv16((int)std::floor(playerWorldPos.z));

	m_tmpWanted.clear();

	// 비용이 커지면 vector 대신 다른거
	size_t newCap = static_cast<size_t>((m_iStreamRadius * 2 + 1) * (m_iStreamRadius * 2 + 1));

	if (m_tmpWanted.capacity() < newCap)
		m_tmpWanted.reserve(newCap);

	for (int dz = -m_iStreamRadius; dz <= m_iStreamRadius; ++dz)
	{
		for (int dx = -m_iStreamRadius; dx <= m_iStreamRadius; ++dx)
		{
			const int cx = centerCx + dx;
			const int cz = centerCz + dz;
			const uint64_t key = MakeColumnKey(cx, cz);

			m_tmpWanted.push_back(key);

			CChunkColumn* column = _FindColumn(cx, cz);
			if (nullptr == column || !column->IsActive())
			{
				_LoadColumn(cx, cz);
			}
		}
	}

	for (auto& kv : m_columns)
	{
		const bool keep = std::find(m_tmpWanted.begin(), m_tmpWanted.end(), kv.first) != m_tmpWanted.end();
		if (false == keep)
		{
			const ChunkCoord coord = kv.second.GetCoord();
			_UnloadColumn(coord.x, coord.z);
		}
	}

	
	_UpdateDebugStats();
}

bool CChunkWorld::PopDirty(SectionCoord& outSectionCoord)
{
	if (m_vecDirtyQueue.empty())
		return false;

	outSectionCoord = m_vecDirtyQueue.back();
	m_vecDirtyQueue.pop_back();
	return true;
}

BlockCell CChunkWorld::GetBlock(int wx, int wy, int wz) const
{
	if (wy < 0 || wy >= CHUNK_SIZE_Y)
		return { 0, 0 };

	BlockCell modified{};
	if (_TryGetModifiedBlock(wx, wy, wz, modified))
		return modified;

	return _GetBaseBlock(wx, wy, wz);
}

bool CChunkWorld::IsSolid(const BlockCell& cell) const
{
	if (cell.IsAir())
		return false;

	return BlockDB.HasCollision(cell.blockID);
}

bool CChunkWorld::SetBlock(int wx, int wy, int wz, const BlockCell& newCell)
{
	if (wy < 0 || wy >= CHUNK_SIZE_Y) 
		return false;

	const BlockCell oldFinal = GetBlock(wx, wy, wz);
	if (oldFinal == newCell)
		return false;

	const BlockCell baseCell = _GetBaseBlock(wx, wy, wz);
	if (newCell == baseCell)
		_EraseModifiedBlock(wx, wy, wz);
	else
		_WriteModifiedBlock(wx, wy, wz, newCell);


	// resident cache
	int cx, sy, cz;
	int lx, ly, lz;

	bool bCurrentSectionRemoved = false;

	if (_WorldToSectionLocal(wx, wy, wz, cx, sy, cz, lx, ly, lz))
	{
		if (CChunkColumn* pColumn = _FindColumn(cx, cz))
		{
			if (pColumn->IsActive())
			{
				CChunkSection* pSection = pColumn->GetSection(sy);

				// Section이 존재하지 않으면 생성
				if (nullptr == pSection)
				{
					pSection = pColumn->EnsureSection(sy);
				}

				if (pSection)
				{
					pSection->SetBlock(lx, ly, lz, newCell);

					if (pSection->IsEmpty())
					{
						_DestoryRenderObjects(*pSection);
						pColumn->ResetSection(sy);
						bCurrentSectionRemoved = true;
					}
					else if (!pSection->HasAnyRenderObjectID() && !pSection->IsEmpty())
					{
						_EnsureRenderObjects(*pSection, cx, sy, cz);
					}
				}
			}
		}
	}

	if (!bCurrentSectionRemoved)
		_MarkDirty(cx, sy, cz);

	if (lx == 0)
		_MarkDirty(cx - 1, sy, cz);
	else if (lx == CHUNK_SIZE_X - 1)
		_MarkDirty(cx + 1, sy, cz);

	if (ly == 0 && sy > 0)
		_MarkDirty(cx, sy - 1, cz);
	else if (ly == CHUNK_SECTION_SIZE - 1 && sy < CHUNK_SECTION_COUNT - 1)
		_MarkDirty(cx, sy + 1, cz);

	if (lz == 0)
		_MarkDirty(cx, sy, cz - 1);
	else if (lz == CHUNK_SIZE_Z - 1)
		_MarkDirty(cx, sy, cz + 1);

	dbg.AddBlockEdit();
	return true;
}

CChunkSection* CChunkWorld::FindSectionDataMutable(int cx, int sy, int cz)
{
	auto* column = _FindColumn(cx, cz);
	if (nullptr == column)
		return nullptr;

	if (sy < 0 || sy >= CHUNK_SECTION_COUNT)
		return nullptr;

	return column->GetSection(sy);
}

const CChunkSection* CChunkWorld::FindSectionData(int cx, int sy, int cz) const
{
	auto* column = _FindColumn(cx, cz);
	if (nullptr == column)
		return nullptr;

	if (sy < 0 || sy >= CHUNK_SECTION_COUNT)
		return nullptr;

	return column->GetSection(sy);
}

CObject* CChunkWorld::FindRenderObject(int cx, int sy, int cz, EChunkSectionRenderSlot slot)
{
	auto* column = _FindColumn(cx, cz);
	if (nullptr == column)
		return nullptr;

	if (sy < 0 || sy >= CHUNK_SECTION_COUNT) 
		return nullptr;

	auto* section = column->GetSection(sy);
	if (nullptr == section)
		return nullptr;

	OBJECT_ID id = section->GetRenderObjectID(slot);
	if (!IsValidObjectID(id)) 
		return nullptr;

	return m_pScene->FindObject(id);
}

bool CChunkWorld::IsSpawnAreaReady(const XMFLOAT3& playerWorldPos) const
{
	const int centerCx = FloorDiv16((int)std::floor(playerWorldPos.x));
	const int centerCz = FloorDiv16((int)std::floor(playerWorldPos.z));

	// flat world 스폰 안정화용: 주변 3x3 컬럼의 section 0만 확인
	for (int dz = -1; dz <= 1; ++dz)
	{
		for (int dx = -1; dx <= 1; ++dx)
		{
			const int cx = centerCx + dx;
			const int cz = centerCz + dz;

			const CChunkColumn* pColumn = _FindColumn(cx, cz);
			if (nullptr == pColumn || !pColumn->IsActive())
				return false;

			const CChunkSection* pSection = pColumn->GetSection(0);
			if (nullptr == pSection)
				return false;

			if (pSection->IsDirty())
				return false;

			if (pSection->IsBuildQueued())
				return false;

			if (!pSection->HasAnyRenderObjectID())
				return false;
		}
	}

	return true;
}

void CChunkWorld::DebugRequestReloadActiveColumns()
{
	if (m_debugReloadPhase != EDebugReloadPhase::NONE)
		return;

	m_debugReloadCoords.clear();
	m_debugReloadCoords.reserve(m_columns.size());

	for (const auto& kv : m_columns)
	{
		const CChunkColumn& column = kv.second;
		if (!column.IsActive())
			continue;

		m_debugReloadCoords.push_back(column.GetCoord());
	}

	if (m_debugReloadCoords.empty())
		return;

	for (const ChunkCoord& coord : m_debugReloadCoords)
	{
		_UnloadColumn(coord.x, coord.z);
	}

	m_debugReloadPhase = EDebugReloadPhase::WAIT_LOAD;
	_UpdateDebugStats();
}

void CChunkWorld::DebugProcessReloadRequest()
{
	if (m_debugReloadCoords.empty())
		return;

	for (const ChunkCoord& coord : m_debugReloadCoords)
	{
		_LoadColumn(coord.x, coord.z);
	}

	m_debugReloadCoords.clear();
	m_debugReloadPhase = EDebugReloadPhase::NONE;
	_UpdateDebugStats();
}

CChunkColumn* CChunkWorld::_FindColumn(int cx, int cz)
{
	auto it = m_columns.find(MakeColumnKey(cx, cz));
	if (it == m_columns.end())
		return nullptr;

	return &it->second;
}

const CChunkColumn* CChunkWorld::_FindColumn(int cx, int cz) const
{
	auto it = m_columns.find(MakeColumnKey(cx, cz));
	if (it == m_columns.end())
		return nullptr;

	return &it->second;
}

CChunkColumn& CChunkWorld::_GetOrCreateColumn(int cx, int cz)
{
	uint64_t key = MakeColumnKey(cx, cz);

	auto it = m_columns.find(key);
	if (it != m_columns.end())
		return it->second;

	auto [newIt, ok] = m_columns.try_emplace(key);
	newIt->second.Initialize(cx, cz);
	return newIt->second;
}

void CChunkWorld::_LoadColumn(int cx, int cz)
{
	CChunkColumn& column = _GetOrCreateColumn(cx, cz);

	if (!column.IsGenerated())
	{
		_GenerateBaseColumn(column);
		column.SetGenerated(true);
	}

	_ApplyModifiedOverlayToColumn(column);

	column.SetResidency(EChunkResidency::ACTIVE);

	for (int sy = 0; sy < CHUNK_SECTION_COUNT; ++sy)
	{
		CChunkSection* pSection = column.GetSection(sy);
		if (nullptr == pSection)
			continue;

		_EnsureRenderObjects(*pSection, cx, sy, cz);
		_MarkDirty(cx, sy, cz);
	}

	dbg.AddChunkLoad();
}

void CChunkWorld::_UnloadColumn(int cx, int cz)
{
	auto* column = _FindColumn(cx, cz);
	if (nullptr == column)
		return;

	if (!column->IsActive())
		return;

	for (int sy = 0; sy < CHUNK_SECTION_COUNT; ++sy)
	{
		CChunkSection* pSection = column->GetSection(sy);
		if (nullptr == pSection)
			continue;

		_DestoryRenderObjects(*pSection);

		pSection->SetBuildQueued(false);
	}

	for (int sy = 0; sy < CHUNK_SECTION_COUNT; ++sy)
	{
		column->ResetSection(sy);
	}

	column->SetResidency(EChunkResidency::RESIDENT);
	column->SetGenerated(false);

	dbg.AddChunkUnload();
}

void CChunkWorld::_EnsureRenderObjects(CChunkSection& section, int cx, int sy, int cz)
{
	_EnsureRenderObject(section, cx, sy, cz, EChunkSectionRenderSlot::OPAQUE_SLOT);
	_EnsureRenderObject(section, cx, sy, cz, EChunkSectionRenderSlot::CUTOUT_SLOT);
	_EnsureRenderObject(section, cx, sy, cz, EChunkSectionRenderSlot::TRANSLUCENT_SLOT);
}

void CChunkWorld::_EnsureRenderObject(CChunkSection& section, int cx, int sy, int cz, EChunkSectionRenderSlot slot)
{
	if (section.HasRenderObjectID(slot))
		return;

	string name = _MakeSectionName(cx, sy, cz, slot);
	CObject* obj = m_pScene->AddAndGetObject(name);

	auto* tr = obj->AddComponent<CTransform>();
	auto* mr = obj->AddComponent<CMeshRenderer>();

	tr->SetLocalTrans(XMFLOAT3(
		(float)(cx * CHUNK_SIZE_X), 
		(float)(sy * CHUNK_SECTION_SIZE), 
		(float)(cz * CHUNK_SIZE_Z))
	);

	switch (slot)
	{
		case EChunkSectionRenderSlot::OPAQUE_SLOT:
		{
			mr->SetPipeline(m_pOpaquePipeline);
			mr->SetMaterial(m_pOpaqueMaterial);
			mr->SetRenderPass(ERenderPass::OPAQUE_PASS);
		} break;

		case EChunkSectionRenderSlot::CUTOUT_SLOT:
		{
			mr->SetPipeline(m_pCutoutPipeline);
			mr->SetMaterial(m_pTranslucentMaterial);
			mr->SetRenderPass(ERenderPass::CUTOUT_PASS);
		} break;

		case EChunkSectionRenderSlot::TRANSLUCENT_SLOT:
		{
			mr->SetPipeline(m_pTranslucentPipeline);
			mr->SetMaterial(m_pTranslucentMaterial);
			mr->SetRenderPass(ERenderPass::TRANSPARENT_PASS);
		} break;
	}

	obj->SetEnable(false);
	section.SetRenderObjectID(slot, obj->GetID());
}

void CChunkWorld::_DestoryRenderObjects(CChunkSection& section)
{
	for (size_t i = 0; i < static_cast<size_t>(EChunkSectionRenderSlot::COUNT); ++i)
	{
		const EChunkSectionRenderSlot slot = static_cast<EChunkSectionRenderSlot>(i);

		if (!section.HasRenderObjectID(slot))
			continue;

		m_pScene->DestroyObject(section.GetRenderObjectID(slot));
		section.ClearRenderObjectID(slot);
	}
}

void CChunkWorld::_MarkDirty(int cx, int sy, int cz)
{
	CChunkSection* pSection = FindSectionDataMutable(cx, sy, cz);
	if (nullptr == pSection)
		return;

	pSection->MarkDirty();

	if (pSection->IsBuildQueued())
		return;

	pSection->SetBuildQueued(true);
	m_vecDirtyQueue.push_back({ cx, sy, cz });
}

bool CChunkWorld::_WorldToSectionLocal(int wx, int wy, int wz, int& outCx, int& outSy, int& outCz, int& outLx, int& outLy, int& outLz) const
{
	if (wy < 0 || wy >= CHUNK_SIZE_Y) 
		return false;

	outCx = FloorDiv16(wx);
	outSy = FloorDiv16(wy);
	outCz = FloorDiv16(wz);

	outLx = Mod16(wx);
	outLy = Mod16(wy);
	outLz = Mod16(wz);

	return true;
}

string CChunkWorld::_MakeSectionName(int cx, int sy, int cz, EChunkSectionRenderSlot slot)
{
	const char* suffix = (slot == EChunkSectionRenderSlot::OPAQUE_SLOT) ? "opaque" : "translucent";

	char buf[96];
	sprintf(buf, "%d_%d_%d_%s", cx, sy, cz, suffix);
	return string(buf);
}

void CChunkWorld::_GenerateBaseColumn(CChunkColumn& column)
{
	if (!m_pGenerator)
		return;

	m_pGenerator->GenerateColumn(column);
}

BlockCell CChunkWorld::_GetBaseBlock(int wx, int wy, int wz) const
{
	if (!m_pGenerator)
		return {0,0};

	return m_pGenerator->SampleBlock(wx, wy, wz);
}

bool CChunkWorld::_TryGetModifiedBlock(int wx, int wy, int wz, BlockCell& outCell) const
{
	if (wy < 0 || wy >= CHUNK_SIZE_Y)
		return false;

	int cx, sy, cz;
	int lx, ly, lz;

	if (!_WorldToSectionLocal(wx, wy, wz, cx, sy, cz, lx, ly, lz))
		return false;

	const uint64_t columnKey = MakeColumnKey(cx, cz);
	auto itCol = m_modifiedColumns.find(columnKey);
	if (itCol == m_modifiedColumns.end())
		return false;

	const uint16_t localIndex = MakeColumnLocalIndex(lx, wy, lz);
	auto itCell = itCol->second.cells.find(localIndex);
	if (itCell == itCol->second.cells.end())
		return false;

	outCell = itCell->second;
	return true;
}

void CChunkWorld::_WriteModifiedBlock(int wx, int wy, int wz, const BlockCell& cell)
{
	int cx, sy, cz;
	int lx, ly, lz;

	if (!_WorldToSectionLocal(wx, wy, wz, cx, sy, cz, lx, ly, lz))
		return;

	const uint64_t columnKey = MakeColumnKey(cx, cz);
	const uint64_t localIndex = MakeColumnLocalIndex(lx, wy, lz);

	m_modifiedColumns[columnKey].cells[localIndex] = cell;
}

void CChunkWorld::_EraseModifiedBlock(int wx, int wy, int wz)
{
	int cx, sy, cz;
	int lx, ly, lz;

	if (!_WorldToSectionLocal(wx, wy, wz, cx, sy, cz, lx, ly, lz))
		return;

	const uint64_t columnKey = MakeColumnKey(cx, cz);
	auto itColumn = m_modifiedColumns.find(columnKey);
	if (itColumn == m_modifiedColumns.end())
		return;

	const uint16_t localIndex = MakeColumnLocalIndex(lx, wy, lz);
	itColumn->second.cells.erase(localIndex);

	if (itColumn->second.cells.empty())
		m_modifiedColumns.erase(itColumn);
}

void CChunkWorld::_ApplyModifiedOverlayToColumn(CChunkColumn& column)
{
	const ChunkCoord coord = column.GetCoord();
	const uint64_t columnKey = MakeColumnKey(coord.x, coord.z);

	auto itCol = m_modifiedColumns.find(columnKey);
	if (itCol == m_modifiedColumns.end())
		return;

	for (const auto& kv : itCol->second.cells)
	{
		const uint16_t idx = kv.first;
		const BlockCell& cell = kv.second;

		const int wy = idx / (CHUNK_SIZE_X * CHUNK_SIZE_Z);
		const int rem = idx % (CHUNK_SIZE_X * CHUNK_SIZE_Z);
		const int lz = rem / CHUNK_SIZE_X;
		const int lx = rem % CHUNK_SIZE_X;

		const int sy = wy / CHUNK_SECTION_SIZE;
		const int ly = wy % CHUNK_SECTION_SIZE;

		CChunkSection* pSection = column.EnsureSection(sy);
		if (nullptr == pSection)
			continue;

		pSection->SetBlock(lx, ly, lz, cell);
	}

	column.SetModified(true);
}

void CChunkWorld::_UpdateDebugStats()
{
	int loadedColumns = 0;
	int loadedSections = 0;
	int dirtySections = 0;

	for (auto& kv : m_columns)
	{
		CChunkColumn& col = kv.second;
		if (!col.IsActive())
			continue;

		++loadedColumns;

		for (int sy = 0; sy < CHUNK_SECTION_COUNT; ++sy)
		{
			CChunkSection* sec = col.GetSection(sy);
			if (!sec) 
				continue;

			++loadedSections;
			if (sec->IsDirty())
				++dirtySections;
		}
	}

	int modifiedColumns = static_cast<int>(m_modifiedColumns.size());
	int modifiedCells = 0;

	for (const auto& kv : m_modifiedColumns)
	{
		modifiedCells += static_cast<int>(kv.second.cells.size());
	}

	dbg.SetLoadedColumnCount(loadedColumns);
	dbg.SetLoadedSectionCount(loadedSections);
	dbg.SetDirtySectionCount(dirtySections);
	dbg.SetRebuildQueuedCount((int)m_vecDirtyQueue.size());

	dbg.SetModifiedColumnCount(modifiedColumns);
	dbg.SetModifiedCellCount(modifiedCells);
}