#include "pch.h"
#include "CChunkColumn.h"

CChunkColumn::CChunkColumn()
{
}

CChunkColumn::~CChunkColumn()
{
}

void CChunkColumn::Initialize(int chunkX, int chunkZ)
{
}

CChunkSection* CChunkColumn::GetSection(int sectionY)
{
	return nullptr;
}

const CChunkSection* CChunkColumn::GetSection(int sectionY) const
{
	return nullptr;
}

BlockCell CChunkColumn::GetBlockCell(int wx, int wy, int wz) const
{

}

void CChunkColumn::SetBlockCell(int wx, int wy, int wz, const BlockCell& cell)
{

}

bool CChunkColumn::_WorldToLocal(int wx, int wy, int wz, XMINT3& outL, int& outSectionY) const
{
	return false;
}
