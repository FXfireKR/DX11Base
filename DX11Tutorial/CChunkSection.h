#pragma once
#include "ChunkTypes.h"

class CChunkSection
{
public:
	CChunkSection();
	~CChunkSection() = default;

	void Clear();
	
	BlockCell GetLocalCell(int lx, int ly, int lz) const;
	void SetLocalCell(int lx, int ly, int lz, const BlockCell& cell);

public:
	inline void MarkMeshDirty() { m_bMeshDirty = true; }
	inline void ClearMeshDirty() { m_bMeshDirty = false; }
	inline const bool IsMeshDirty() const { return m_bMeshDirty; }
	inline const bool IsEmpty() const { return m_bEmpty; }

private:
	int _ToIndex(int lx, int ly, int lz) const;
	
private:
	array<BlockCell, CHUNK_SECTION_VOLUME> m_cells{};
	bool m_bMeshDirty = false;
	bool m_bEmpty = true;
};