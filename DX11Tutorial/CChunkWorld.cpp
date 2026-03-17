#include "pch.h"
#include "CChunkWorld.h"
#include "CScene.h"
#include "CPipeline.h"
#include "CMaterial.h"
#include "CWorld.h"
#include "ChunkMath.h"

using namespace ChunkMath;

void CChunkWorld::Initialize(CScene& scene, CPipeline& pipeline, CMaterial& material)
{
	m_pScene = &scene;
	m_pPipeline = &pipeline;
	m_pMaterial = &material;
}

void CChunkWorld::UpdateStreaming(const XMFLOAT3& playerWorldPos)
{
	const int centerCx = FloorDiv16((int)std::floor(playerWorldPos.x));
	const int centerCz = FloorDiv16((int)std::floor(playerWorldPos.z));

	// 비용이 커지면 vector 대신 다른거
	vector<uint64_t> wanted;
	wanted.reserve((m_iStreamRadius * 2 + 1) * (m_iStreamRadius * 2 + 1));

	for (int dz = -m_iStreamRadius; dz <= m_iStreamRadius; ++dz)
	{
		for (int dx = -m_iStreamRadius; dx <= m_iStreamRadius; ++dx)
		{
			const int cx = centerCx + dx;
			const int cz = centerCz + dz;
			const uint64_t key = MakeColumnKey(cx, cz);

			wanted.push_back(key);

			if (m_columns.find(key) == m_columns.end())
				_LoadColumn(cx, cz);
		}
	}

	for (auto it = m_columns.begin(); it != m_columns.end();)
	{
		const bool keep = std::find(wanted.begin(), wanted.end(), it->first) != wanted.end();
		if (false == keep) 
		{
			const ChunkCoord coord = it->second.GetCoord();
			++it;
			_UnloadColumn(coord.x, coord.z);
		}
		else
		{
			++it;
		}
	}
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

	int cx, sy, cz;
	int lx, ly, lz;

	if (!_WorldToSectionLocal(wx, wy, wz, cx, sy, cz, lx, ly, lz))
		return { 0, 0 };

	const CChunkColumn* pColumn = _FindColumn(cx, cz);
	if (nullptr == pColumn)
		return { 0, 0 };

	const CChunkSection* pSection = pColumn->GetSection(sy);
	if (nullptr == pSection)
		return { 0, 0 };

	return pSection->GetBlock(lx, ly, lz);
}

bool CChunkWorld::IsSolid(const BlockCell& cell) const
{
	return !cell.IsAir();
}

bool CChunkWorld::SetBlock(int wx, int wy, int wz, const BlockCell& newCell)
{
	if (wy < 0 || wy >= CHUNK_SIZE_Y) 
		return false;

	int cx, sy, cz;
	int lx, ly, lz;

	if (!_WorldToSectionLocal(wx, wy, wz, cx, sy, cz, lx, ly, lz))
		return false;

	CChunkColumn* pColumn = _FindColumn(cx, cz);
	if (nullptr == pColumn)
		return false;

	CChunkSection* pSection = pColumn->GetSection(sy);
	if (nullptr == pSection)
		return false;

	const BlockCell oldCell = pSection->GetBlock(lx, ly, lz);
	if (oldCell == newCell) 
		return false;

	pSection->SetBlock(lx, ly, lz, newCell);
	_MarkDirty(cx, sy, cz);

	if (lx == 0)
		_MarkDirty(cx - 1, sy, cz);
	else if (lx == CHUNK_SIZE_X - 1)
		_MarkDirty(cx + 1, sy, cz);

	if (ly == 0 && sy > 0)
		_MarkDirty(cx, sy - 1, cz);
	else if (ly == CHUNK_SECTION_COUNT - 1 && sy < CHUNK_SECTION_COUNT - 1)
		_MarkDirty(cx, sy + 1, cz);

	if (lz == 0)
		_MarkDirty(cx, sy, cz - 1);
	else if (lz == CHUNK_SIZE_Z - 1)
		_MarkDirty(cx, sy, cz + 1);

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

CObject* CChunkWorld::FindRenderObject(int cx, int sy, int cz)
{
	auto* column = _FindColumn(cx, cz);
	if (nullptr == column)
		return nullptr;

	if (sy < 0 || sy >= CHUNK_SECTION_COUNT) 
		return nullptr;

	auto* section = column->GetSection(sy);
	if (nullptr == section)
		return nullptr;

	OBJECT_ID id = section->GetRenderObjectID();
	if (!IsValidObjectID(id)) 
		return nullptr;

	return m_pScene->FindObject(id);
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
	_GenerateFlatTestColumn(column);

	for (int sy = 0; sy < CHUNK_SECTION_COUNT; ++sy)
	{
		CChunkSection* pSection = column.GetSection(sy);
		if (nullptr == pSection)
			continue;

		_EnsureRenderObject(*pSection, cx, sy, cz);
		_MarkDirty(cx, sy, cz);
	}
}

void CChunkWorld::_UnloadColumn(int cx, int cz)
{
	auto* column = _FindColumn(cx, cz);
	if (nullptr == column)
		return;

	for (int sy = 0; sy < CHUNK_SECTION_COUNT; ++sy)
	{
		CChunkSection* pSection = column->GetSection(sy);
		if (nullptr == pSection)
			continue;

		_DestoryRenderObject(*pSection);
	}

	m_columns.erase(MakeColumnKey(cx, cz));
}

void CChunkWorld::_GenerateFlatTestColumn(CChunkColumn& column)
{
	const ChunkCoord coord = column.GetCoord();
	CChunkSection* pSection = column.EnsureSection(0);

	if (nullptr == pSection)
		return;

	BLOCK_ID stone = BlockDB.FindBlockID("minecraft:stone");
	BlockPropHashMap props;
	STATE_INDEX sidx{};
	bool ok = BlockDB.EncodeStateIndex(stone, props, sidx);
	assert(ok);

	const int baseWx = coord.x * CHUNK_SIZE_X;
	const int baseWz = coord.z * CHUNK_SIZE_Z;

	for (int lz = 0; lz < CHUNK_SIZE_Z; ++lz)
		for (int lx = 0; lx < CHUNK_SIZE_X; ++lx)
			pSection->SetBlock(lx, 0, lz, { stone, sidx });
}

void CChunkWorld::_EnsureRenderObject(CChunkSection& section, int cx, int sy, int cz)
{
	if (section.HasRenderObjectID())
		return;

	string name = _MakeSectionName(cx, sy, cz);
	CObject* obj = m_pScene->AddAndGetObject(name);

	auto* tr = obj->AddComponent<CTransform>();
	auto* mr = obj->AddComponent<CMeshRenderer>();

	tr->SetLocalTrans(XMFLOAT3((float)(cx * CHUNK_SIZE_X), (float)(sy * CHUNK_SECTION_SIZE), (float)(cz * CHUNK_SIZE_Z)));

	// 기본 파이프라인/메터리얼 세팅
	mr->SetPipeline(m_pPipeline);
	mr->SetMaterial(m_pMaterial);

	section.SetRenderObjectID(obj->GetID());
}

void CChunkWorld::_DestoryRenderObject(CChunkSection& section)
{
	if (!section.HasRenderObjectID())
		return;

	m_pScene->DestroyObject(section.GetRenderObjectID());
	section.ClearRenderObjectID();
}

void CChunkWorld::_MarkDirty(int cx, int sy, int cz)
{
	auto* column = _FindColumn(cx, cz);
	if (nullptr == column)
		return;

	auto* section = column->GetSection(sy);
	if (nullptr == section)
		return;

	section->MarkDirty();

	if (section->IsBuildQueued())
		return;

	section->SetBuildQueued(true);
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

string CChunkWorld::_MakeSectionName(int cx, int sy, int cz)
{
	char buf[64];
	sprintf(buf, "%d_%d_%d", cx, sy, cz);
	return string(buf);
}
