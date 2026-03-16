#pragma once
#include "ObjectTypes.h"
#include "CChunkSection.h"

class CChunkColumn
{
public:
	CChunkColumn();
	~CChunkColumn();

	void Initialize(int chunkX, int chunkZ);

	CChunkSection* GetSection(int sectionY);
	const CChunkSection* GetSection(int sectionY) const;

	BlockCell GetBlockCell(int wx, int wy, int wz) const;
	void SetBlockCell(int wx, int wy, int wz, const BlockCell& cell);

public:
	inline int GetChunkX() const { return m_iChunkX; }
	inline int GetChunkZ() const { return m_iChunkZ; }

private:
	bool _WorldToLocal(int wx, int wy, int wz, XMINT3& outL, int& outSectionY) const;

private:
	int m_iChunkX = 0;
	int m_iChunkZ = 0;

	array<unique_ptr<CChunkSection>, CHUNK_SECTION_COUNT> m_sections;
};