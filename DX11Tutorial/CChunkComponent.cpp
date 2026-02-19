#include "pch.h"
#include "CChunkComponent.h"

void CChunkComponent::Init()
{
	m_arrayBlocks.fill(0);
	m_bDirty = true;
}

BLOCK_ID CChunkComponent::GetBlock(int x, int y, int z) const
{
	if (!InChunk(x, y, z)) return 0;
	return m_arrayBlocks[GetChunkIndex(x, y, z)];
}

void CChunkComponent::SetBlock(int x, int y, int z, BLOCK_ID id)
{
	if (!InChunk(x, y, z)) return;
	m_arrayBlocks[GetChunkIndex(x, y, z)] = id;
	m_bDirty = true;
}
