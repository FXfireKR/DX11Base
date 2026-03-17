#include "pch.h"
#include "CChunkColumn.h"

CChunkSection* CChunkColumn::GetSection(int sy)
{
	if (sy < 0 || sy >= CHUNK_SECTION_SIZE)
		return nullptr;

	return m_sections[sy].get();
}

const CChunkSection* CChunkColumn::GetSection(int sy) const
{
	if (sy < 0 || sy >= CHUNK_SECTION_SIZE)
		return nullptr;

	return m_sections[sy].get();
}