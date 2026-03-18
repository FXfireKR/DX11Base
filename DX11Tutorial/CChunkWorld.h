#pragma once
#include "CChunkColumn.h"
#include "IBlockAccessor.hpp"

class CScene;
class CObject;
class CPipeline;
class CMaterial;

/*

CWorld
 └─ CChunkWorld : public IBlockAccessor
	 └─ unordered_map<ColumnKey, CChunkColumn>
		 └─ array<unique_ptr<CChunkSection>, CHUNK_SECTION_COUNT>

CChunkSection
 ├─ array<BlockCell, CHUNK_SECTION_VOLUME> m_cells
 ├─ bool m_bDirty
 ├─ bool m_bBuildQueued
 └─ OBJECT_ID m_renderObjectID

*/

class CChunkWorld : public IBlockAccessor
{
public:
	CChunkWorld() = default;
	~CChunkWorld() = default;

	void Initialize(CScene& scene, CPipeline* pipeline, CMaterial* material);
	void UpdateStreaming(const XMFLOAT3& playerWorldPos);
	bool PopDirty(SectionCoord& outSectionCoord);

	// IBlockAccessor
	BlockCell GetBlock(int wx, int wy, int wz) const override;
	bool IsSolid(const BlockCell& cell) const override;

	bool SetBlock(int wx, int wy, int wz, const BlockCell& newCell);

	CChunkSection* FindSectionDataMutable(int cx, int sy, int cz);
	const CChunkSection* FindSectionData(int cx, int sy, int cz) const;
	CObject* FindRenderObject(int cx, int sy, int cz);

public:
	template<typename Fn>
	void ForEachLoadedColumn(Fn&& fn) const
	{
		for (const auto& kv : m_columns)
		{
			fn(kv.second);
		}
	}

private:
	CChunkColumn* _FindColumn(int cx, int cz);
	const CChunkColumn* _FindColumn(int cx, int cz) const;
	CChunkColumn& _GetOrCreateColumn(int cx, int cz);

	void _LoadColumn(int cx, int cz);
	void _UnloadColumn(int cx, int cz);

	void _GenerateFlatTestColumn(CChunkColumn& column);
	void _EnsureRenderObject(CChunkSection& section, int cx, int sy, int cz);
	void _DestoryRenderObject(CChunkSection& section);
	
	void _MarkDirty(int cx, int sy, int cz);
	bool _WorldToSectionLocal(int wx, int wy, int wz,
		int& outCx, int& outSy, int& outCz,
		int& outLx, int& outLy, int& outLz) const;

	string _MakeSectionName(int cx, int sy, int cz);

public:
	inline const int GetStreamRadius() const { return m_iStreamRadius; }
	inline void SetStreamRadius(int iRadius) { m_iStreamRadius = iRadius > 0 ? iRadius : m_iStreamRadius; }

private:
	CScene* m_pScene = nullptr;
	CPipeline* m_pPipeline = nullptr;
	CMaterial* m_pMaterial = nullptr;

	unordered_map<uint64_t, CChunkColumn> m_columns;
	vector<SectionCoord> m_vecDirtyQueue;

	vector<uint64_t> m_tmpWanted;
	int m_iStreamRadius = 2;
};

//using namespace ChunkMath;
//
//void CChunkWorld::Initialize(CScene& scene, CPipeline* pPipeline, CMaterial* pMaterial)
//{
//	m_pScene = &scene;
//	m_pPipeline = pPipeline;
//	m_pMaterial = pMaterial;
//}
//
//BlockCell CChunkWorld::GetBlockCell(int wx, int wy, int wz) const
//{
//	int cx, sy, cz;
//	int lx, ly, lz;
//
//	if (!_WorldToSectionLocal(wx, wy, wz, cx, sy, cz, lx, ly, lz))
//		return { 0, 0 };
//
//	const ChunkSection* pSection = _FindSectionData(cx, sy, cz);
//	if (!pSection) return { 0, 0 };
//
//	return pSection->blocks[IndexSection(lx, ly, lz)];
//}
//
//BlockCell CChunkWorld::GetBlockCell(XMINT3 w) const
//{
//	return GetBlockCell(w.x, w.y, w.z);
//}
//
//bool CChunkWorld::IsSolid(const BlockCell& cell) const
//{
//	return !cell.IsAir();
//}
//
//BLOCK_ID CChunkWorld::GetBlock(int wx, int wy, int wz) const
//{
//	return GetBlockCell(wx, wy, wz).blockID;
//}
//
//void CChunkWorld::SetBlock(int wx, int wy, int wz, const BlockCell& block)
//{
//	if (wy < 0 || wy >= CHUNK_SIZE_Y) return;
//
//	int cx, sy, cz;
//	int lx, ly, lz;
//
//	if (!_WorldToSectionLocal(wx, wy, wz, cx, sy, cz, lx, ly, lz)) 
//		return;
//
//	ChunkSection* pSection = _GetOrCreateSectionData(cx, sy, cz);
//	if (!pSection) return;
//
//	BlockCell& dst = pSection->blocks[IndexSection(lx, ly, lz)];
//	if (dst == block) return;
//
//	dst = block;
//	_MarkDirty(cx, sy, cz);
//
//	if (lx == 0) _MarkDirty(cx - 1, sy, cz);
//	else if (lx == CHUNK_SIZE_X - 1) _MarkDirty(cx + 1, sy, cz);
//	if (ly == 0 && sy > 0) _MarkDirty(cx, sy - 1, cz);
//	else if (ly == CHUNK_SECTION_COUNT - 1 && sy < CHUNK_SECTION_COUNT - 1) _MarkDirty(cx, sy + 1, cz);
//	if (lz == 0) _MarkDirty(cx, sy, cz - 1);
//	else if (lz == CHUNK_SIZE_Z - 1) _MarkDirty(cx, sy, cz + 1);
//}
//
//void CChunkWorld::EnsureRadiusLoaded(int centerWx, int centerWz, int radiusColumns)
//{
//	int pcx = FloorDiv16(centerWx);
//	int pcz = FloorDiv16(centerWz);
//
//	m_tmpWanted.clear();
//	m_tmpWanted.reserve((2 * radiusColumns + 1) * (2 * radiusColumns + 1));
//
//	for (int dz = -radiusColumns; dz <= radiusColumns; ++dz)
//	{
//		for (int dx = -radiusColumns; dx <= radiusColumns; ++dx)
//		{
//			int cx = pcx + dx;
//			int cz = pcz + dz;
//			uint64_t key = _MakeColumnKey(cx, cz);
//			m_tmpWanted.push_back(key);
//
//			if (m_mapColumns.find(key) == m_mapColumns.end())
//				_LoadColumn(cx, cz);
//		}
//	}
//
//	// 멀어진 컬럼 언로드
//	// TODO
//}
//
//bool CChunkWorld::PopDirty(OUT ChunkCoord& coord)
//{
//	if (m_dirtyQueue.empty()) return false;
//	coord = m_dirtyQueue.back();
//	m_dirtyQueue.pop_back();
//	return true;
//}
//
//ChunkSection* CChunkWorld::FindSectionDataMutable(int cx, int sy, int cz)
//{
//	auto* col = _FindColumn(cx, cz);
//	if (!col) return nullptr;
//	if (sy < 0 || sy >= CHUNK_SECTION_COUNT) return nullptr;
//	return col->sections[sy].get();
//}
//
//const ChunkSection* CChunkWorld::FindSectionData(int cx, int sy, int cz) const
//{
//	auto* col = _FindColumn(cx, cz);
//	if (!col) return nullptr;
//	if (sy < 0 || sy >= CHUNK_SECTION_COUNT) return nullptr;
//	return col->sections[sy].get();
//}
//
//CObject* CChunkWorld::FindRenderObject(int cx, int sy, int cz)
//{
//	auto* col = _FindColumn(cx, cz);
//	if (!col) return nullptr;
//	if (sy < 0 || sy >= CHUNK_SECTION_COUNT) return nullptr;
//
//	OBJECT_ID id = col->renderObj[sy];
//	if (!IsValidObjectID(id)) return nullptr;
//	return m_pScene->FindObject(id);
//}
//
//ChunkColumn* CChunkWorld::_FindColumn(int cx, int cz)
//{
//	auto it = m_mapColumns.find(_MakeColumnKey(cx, cz));
//	return (it == m_mapColumns.end()) ? nullptr : &it->second;
//}
//
//const ChunkColumn* CChunkWorld::_FindColumn(int cx, int cz) const
//{
//	auto it = m_mapColumns.find(_MakeColumnKey(cx, cz));
//	return (it == m_mapColumns.end()) ? nullptr : &it->second;
//}
//
//ChunkSection* CChunkWorld::_GetOrCreateSectionData(int cx, int sy, int cz)
//{
//	ChunkColumn& col = _GetOrCreateColumn(cx, cz);
//	if (!col.sections[sy])
//		col.sections[sy] = make_unique<ChunkSection>();
//	_EnsureRenderObject(col, sy);
//	return col.sections[sy].get();
//}
//
//ChunkSection* CChunkWorld::_FindSectionData(int cx, int sy, int cz)
//{
//	const ChunkColumn* col = _FindColumn(cx, cz);
//	if (!col) return nullptr;
//	return col->sections[sy].get();
//}
//
//const ChunkSection* CChunkWorld::_FindSectionData(int cx, int sy, int cz) const
//{
//	const ChunkColumn* col = _FindColumn(cx, cz);
//	if (!col) return nullptr;
//	return col->sections[sy].get();
//}
//
//ChunkColumn& CChunkWorld::_GetOrCreateColumn(int cx, int cz)
//{
//	const uint64_t key = _MakeColumnKey(cx, cz);
//	auto it = m_mapColumns.find(key);
//	if (it != m_mapColumns.end()) return it->second;
//
//	ChunkColumn col{};
//	col.coord = { cx, cz };
//	for (int i = 0; i < 16; ++i) {
//		col.renderObj[i] = INVALID_OBJECT_ID;
//	}
//	auto [newIt, ok] = m_mapColumns.emplace(key, move(col));
//	return newIt->second;
//}
//
//void CChunkWorld::_LoadColumn(int cx, int cz)
//{
//	_GetOrCreateColumn(cx, cz);
//}
//
//void CChunkWorld::_UnloadColumn(ChunkColumn& col)
//{
//	for (int sy = 0; sy < 16; ++sy)
//	{
//		if (IsValidObjectID(col.renderObj[sy]))
//			m_pScene->DestroyObject(col.renderObj[sy]);
//		col.renderObj[sy] = INVALID_OBJECT_ID;
//		col.sections[sy].reset();
//	}
//}
//
//void CChunkWorld::_EnsureRenderObject(ChunkColumn& col, int sy)
//{
//	if (IsValidObjectID(col.renderObj[sy])) return;
//
//	const int cx = col.coord.cx;
//	const int cz = col.coord.cz;
//
//	string name = _MakeSectionName(cx, sy, cz);
//	CObject* obj = m_pScene->AddAndGetObject(name);
//
//	auto* tr = obj->AddComponent<CTransform>();
//	auto* mr = obj->AddComponent<CMeshRenderer>();
//
//	tr->SetLocalTrans(XMFLOAT3((float)(cx * CHUNK_SIZE_X), (float)(sy * CHUNK_SECTION_SIZE), (float)(cz * CHUNK_SIZE_Z)));
//
//	// 기본 파이프라인/메터리얼 세팅
//	mr->SetPipeline(m_pPipeline);
//	mr->SetMaterial(m_pMaterial);
//
//	col.renderObj[sy] = obj->GetID();
//}
//
//void CChunkWorld::_MarkDirty(int cx, int sy, int cz)
//{
//	if (sy < 0 || sy >= CHUNK_SECTION_COUNT) return;
//
//	ChunkColumn* col = _FindColumn(cx, cz);
//	if (!col) return;
//	if (!col->sections[sy]) return;
//
//	ChunkSection* sec = col->sections[sy].get();
//	if (!sec) return;
//
//	sec->dirty = true;
//	if (sec->dirtyQueued) return;
//	sec->dirtyQueued = true;
//
//	m_dirtyQueue.push_back({ cx, sy, cz });
//}