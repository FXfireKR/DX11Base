#include "pch.h"
#include "CChunkColumn.h"

void CChunkColumn::Initialize(int cx, int cz)
{
	m_coord = { cx, cz };
}

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

CChunkSection* CChunkColumn::EnsureSection(int sy)
{
	if (sy < 0 || sy >= CHUNK_SECTION_SIZE)
		return nullptr;

	if (!m_sections[sy])
		m_sections[sy] = make_unique<CChunkSection>();

	return m_sections[sy].get();
}
