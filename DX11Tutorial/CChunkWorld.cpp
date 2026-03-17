#include "pch.h"
#include "CChunkWorld.h"
#include "CScene.h"
#include "CPipeline.h"
#include "CMaterial.h"
#include "CWorld.h"
#include "ChunkMath.h"

using namespace ChunkMath;

void CChunkWorld::Initialize(CScene& scene, CPipeline* pPipeline, CMaterial* pMaterial)
{

}

void CChunkWorld::UpdateStreaming(int centerWx, int centerWz)
{

}

bool CChunkWorld::PopDirty(SectionCoord& outSectionCoord)
{
	if (m_vecDirtyQueue.empty())
		return false;

	outSectionCoord = m_vecDirtyQueue.back();
	m_vecDirtyQueue.pop_back();
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


BlockCell CChunkWorld::GetBlock(int wx, int wy, int wz) const
{
	int cx, sy, cz;
	int lx, ly, lz;

	if (!_WorldToSectionLocal(wx, wy, wz, cx, sy, cz, lx, ly, lz))
		return { 0, 0 };

	const CChunkSection* pSection = _FindSectionData(cx, sy, cz);
	if (nullptr == pSection) 
		return { 0, 0 };

	return pSection->GetBlock(lx, ly, lz);
}

BlockCell CChunkWorld::GetBlock(XMINT3 w) const
{
	return GetBlock(w.x, w.y, w.z);
}

bool CChunkWorld::IsSolid(const BlockCell& cell) const
{
	return !cell.IsAir();
}

bool CChunkWorld::SetBlock(int wx, int wy, int wz, const BlockCell& newCell)
{
	if (wy < 0 || wy >= CHUNK_SIZE_Y) return;

	int cx, sy, cz;
	int lx, ly, lz;

	if (!_WorldToSectionLocal(wx, wy, wz, cx, sy, cz, lx, ly, lz)) 
		return;

	CChunkSection* pSection = _GetOrCreateSectionData(cx, sy, cz);
	if (!pSection) return;

	BlockCell dst = pSection->GetBlock(lx, ly, lz);
	if (dst == newCell) return;

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

CChunkSection* CChunkWorld::_GetOrCreateSectionData(int cx, int sy, int cz)
{
	CChunkColumn& column = _GetOrCreateColumn(cx, cz);
	if (!column.GetSection(sy))
	{
		column.GetSection(sy) = make_unique<CChunkSection>();
	}

	_EnsureRenderObject(column, sy);
	return column.GetSection(sy);
}

CChunkSection* CChunkWorld::_FindSectionData(int cx, int sy, int cz)
{
	return nullptr;
}

const CChunkSection* CChunkWorld::_FindSectionData(int cx, int sy, int cz) const
{
	return nullptr;
}

CChunkColumn& CChunkWorld::_GetOrCreateColumn(int cx, int cz)
{
	// TODO: 여기에 return 문을 삽입합니다.
}

void CChunkWorld::_LoadColumn(int cx, int cz)
{
}

void CChunkWorld::_UnloadColumn(int cx, int cz)
{
}

void CChunkWorld::_EnsureRenderObject(CChunkColumn& column, int sy)
{
}

void CChunkWorld::_MarkDirty(int cx, int sy, int cz)
{
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