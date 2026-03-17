#pragma once
#include "CChunkSection.h"

class CChunkColumn
{
public:
	CChunkColumn() = default;
	~CChunkColumn() = default;

	CChunkSection* GetSection(int sy);
	const CChunkSection* GetSection(int sy) const;

	CChunkSection* EnsureSection(int sy);

public:
	inline const ChunkCoord& GetCoord() const { return m_coord; }
	


private:
	array<unique_ptr<CChunkSection>, CHUNK_SECTION_COUNT> m_sections;
	ChunkCoord m_coord;
};