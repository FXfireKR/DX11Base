#include "pch.h"
#include "CChunkSection.h"

CChunkSection::CChunkSection()
{
    m_cells.fill(BlockCell{});
    m_bDirty = true;
    m_renderObjectID = INVALID_OBJECT_ID;
}

BlockCell CChunkSection::GetBlock(int lx, int ly, int lz) const
{ 
    // TODO : 복사해서 넘길 이유가 없다면 나중에 const-ref로 바꾸자.
    return m_cells[_ToIndex(lx, ly, lz)];
}

void CChunkSection::SetBlock(int lx, int ly, int lz, const BlockCell& cell)
{
    m_cells[_ToIndex(lx, ly, lz)] = cell;
    m_bDirty = true;
}

int CChunkSection::_ToIndex(int lx, int ly, int lz) const
{
    return lx + (lz * CHUNK_SECTION_SIZE) + (ly * CHUNK_SECTION_SIZE * CHUNK_SECTION_SIZE);
}