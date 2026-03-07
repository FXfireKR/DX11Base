#pragma once

class CScene;
class CObject;
class CPipeline;
class CMaterial;

class CChunkWorld : public IBlockAccessor
{
public:
	CChunkWorld() = default;
	~CChunkWorld() = default;

	void Initialize(CScene& scene, CPipeline* pPipeline, CMaterial* pMaterial);

	BlockCell GetBlockCell(int wx, int wy, int wz) const override;
	bool IsSolid(const BlockCell& cell) const override;
	

	BLOCK_ID GetBlock(int wx, int wy, int wz) const;
	void SetBlock(int wx, int wy, int wz, const BlockCell& block);

	void EnsureRadiusLoaded(int centerWx, int centerWz, int radiusColumns);
	bool PopDirty(OUT ChunkCoord& coord);

	ChunkSection* FindSectionDataMutable(int cx, int sy, int cz);
	const ChunkSection* FindSectionData(int cx, int sy, int cz) const;
	CObject* FindRenderObject(int cx, int sy, int cz);

private:
	ChunkColumn* _FindColumn(int cx, int cz);
	const ChunkColumn* _FindColumn(int cx, int cz) const;

	ChunkSection* _GetOrCreateSectionData(int cx, int sy, int cz);
	ChunkSection* _FindSectionData(int cx, int sy, int cz);
	const ChunkSection* _FindSectionData(int cx, int sy, int cz) const;

	ChunkColumn& _GetOrCreateColumn(int cx, int cz);
	void _LoadColumn(int cx, int cz);
	void _UnloadColumn(ChunkColumn& col);
	void _EnsureRenderObject(ChunkColumn& col, int sy);
	void _MarkDirty(int cx, int sy, int cz);

	bool _WorldToSectionLocal(int wx, int wy, int wz
		, int& outCx, int& outSy, int& outCz
		,int& outLx, int& outLy, int& outLz) const;

private:
	inline const uint64_t _MakeColumnKey(int cx, int cz) const {
		return (uint64_t)(uint32_t)cx | ((uint64_t)(uint32_t)cz << 32);
	}

	inline string _MakeSectionName(int cx, int sy, int cz) {
		char buf[64];
		sprintf_s(buf, "Section_%d_%d_%d", cx, sy, cz);
		return std::string(buf);
	}


private:
	CScene* m_pScene = nullptr;
	CPipeline* m_pPipeline = nullptr;
	CMaterial* m_pMaterial = nullptr;

	unordered_map<uint64_t, ChunkColumn> m_mapColumns;
	vector<ChunkCoord> m_dirtyQueue;

	vector<uint64_t> m_tmpWanted;
};