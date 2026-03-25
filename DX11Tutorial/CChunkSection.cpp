#include "pch.h"
#include "CChunkSection.h"

CChunkSection::CChunkSection()
{
    m_cells.fill(BlockCell{});
    m_nonAirCount = 0;
    m_bDirty = true;
    m_bBuildQueued = false;
    ClearAllRenderObjectIDs();
}

BlockCell CChunkSection::GetBlock(int lx, int ly, int lz) const
{ 
    // TODO : 복사해서 넘길 이유가 없다면 나중에 const-ref로 바꾸자.
    return m_cells[_ToIndex(lx, ly, lz)];
}

void CChunkSection::SetBlock(int lx, int ly, int lz, const BlockCell& cell)
{
    BlockCell& oldCell = m_cells[_ToIndex(lx, ly, lz)];
    if (oldCell == cell)
        return;

    const bool wasAir = oldCell.IsAir();
    const bool isAir = cell.IsAir();

    oldCell = cell;
    if (wasAir != isAir)
    {
        m_nonAirCount += isAir ? -1 : 1;
    }

    m_bDirty = true;
}

int CChunkSection::_ToIndex(int lx, int ly, int lz) const
{
    return lx + (lz * CHUNK_SECTION_SIZE) + (ly * CHUNK_SECTION_SIZE * CHUNK_SECTION_SIZE);
}