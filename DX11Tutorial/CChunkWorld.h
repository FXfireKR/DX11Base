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

struct ModifiedColumnOverlay
{
	unordered_map<uint16_t, BlockCell> cells;
};

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

	// range-based accessor
	template<typename Fn>
	void ForEachResidentColumn(Fn&& fn) const {
		for (const auto& kv : m_columns)
			fn(kv.second);
	}

	template<typename Fn>
	void ForEachActiveColumn(Fn&& fn) const {
		for (const auto& kv : m_columns)
			if (kv.second.IsActive())
				fn(kv.second);
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

	BlockCell _SampleBaseBlock(int wx, int wy, int wz) const;
	BlockCell _GetBaseBlock(int wx, int wy, int wz) const;
	bool _TryGetModifiedBlock(int wx, int wy, int wz, BlockCell& outCell) const;

	void _WriteModifiedBlock(int wx, int wy, int wz, const BlockCell& cell);
	void _EraseModifiedBlock(int wx, int wy, int wz);

	void _ApplyModifiedOverlayToColumn(CChunkColumn& column);

public:
	inline const int GetStreamRadius() const { return m_iStreamRadius; }
	inline void SetStreamRadius(int iRadius) { m_iStreamRadius = iRadius > 0 ? iRadius : m_iStreamRadius; }

private:
	CScene* m_pScene = nullptr;
	CPipeline* m_pPipeline = nullptr;
	CMaterial* m_pMaterial = nullptr;

	unordered_map<uint64_t, CChunkColumn> m_columns;
	unordered_map<uint64_t, ModifiedColumnOverlay> m_modifiedColumns;

	vector<SectionCoord> m_vecDirtyQueue;
	vector<uint64_t> m_tmpWanted;
	int m_iStreamRadius = 2;
};