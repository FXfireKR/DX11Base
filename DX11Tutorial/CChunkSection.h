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

	inline const bool IsBuildQueued() const { return m_bBuildQueued; }
	inline void SetBuildQueued(bool bQueued) { m_bBuildQueued = bQueued; }

	inline bool HasRenderObjectID() const { return IsValidObjectID(m_renderObjectID); }
	inline const OBJECT_ID GetRenderObjectID() const { return m_renderObjectID; }
	inline void SetRenderObjectID(OBJECT_ID id) { m_renderObjectID = id; }
	inline void ClearRenderObjectID() { m_renderObjectID = INVALID_OBJECT_ID; }

	inline bool IsEmpty() const { return m_nonAirCount == 0; }

private:
	int _ToIndex(int lx, int ly, int lz) const;
	
private:
	array<BlockCell, CHUNK_SECTION_VOLUME> m_cells{};
	uint16_t m_nonAirCount = 0;


	bool m_bDirty = true;
	bool m_bBuildQueued = false;

	// handle
	OBJECT_ID m_renderObjectID = INVALID_OBJECT_ID;
};