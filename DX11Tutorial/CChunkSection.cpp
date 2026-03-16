#include "pch.h"
#include "CChunkSection.h"

CChunkSection::CChunkSection()
{
    Clear();
}

void CChunkSection::Clear()
{
    m_cells.fill(BlockCell());

    m_bMeshDirty = false;
    m_bEmpty = true;
}

BlockCell CChunkSection::GetLocalCell(int lx, int ly, int lz) const
{ // TODO : 복사해서 넘길 이유가 없다면 나중에 const-ref로 바꾸자.
    return m_cells[_ToIndex(lx, ly, lz)];
}

void CChunkSection::SetLocalCell(int lx, int ly, int lz, const BlockCell& cell)
{
    m_cells[_ToIndex(lx, ly, lz)] = cell;

    if (cell.blockID != 0)
        m_bEmpty = false;

    m_bMeshDirty = true;
}

int CChunkSection::_ToIndex(int lx, int ly, int lz) const
{
    return lx + (lz * CHUNK_SECTION_SIZE) + (ly * CHUNK_SECTION_SIZE * CHUNK_SECTION_SIZE);
}