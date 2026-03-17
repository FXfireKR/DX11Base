#pragma once
#include "ChunkTypes.h"

class CChunkSection
{
public:
	CChunkSection();
	~CChunkSection() = default;

	BlockCell GetBlock(int lx, int ly, int lz) const;
	void SetBlock(int lx, int ly, int lz, const BlockCell& cell);

public:
	inline void MarkDirty() { m_bDirty = true; }
	inline void ClearDirty() { m_bDirty = false; }
	inline const bool IsDirty() const { return m_bDirty; }

	inline bool HasRenderObjectID() const { return IsValidObjectID(m_renderObjectID); }
	inline const OBJECT_ID GetRenderObjectID() const { return m_renderObjectID; }
	inline void SetRenderObjectID(OBJECT_ID id) { m_renderObjectID = id; }
	inline void ClearRenderObjectID() { m_renderObjectID = INVALID_OBJECT_ID; }

private:
	int _ToIndex(int lx, int ly, int lz) const;
	
private:
	array<BlockCell, CHUNK_SECTION_VOLUME> m_cells;
	bool m_bDirty = true;

	// handle
	OBJECT_ID m_renderObjectID = INVALID_OBJECT_ID;
};