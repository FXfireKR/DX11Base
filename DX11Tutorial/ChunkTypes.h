#pragma once
#include <array>
#include <memory>
#include <DirectXMath.h>

#include "VoxelTypes.h"


static constexpr int CHUNK_SIZE_X = 16;
static constexpr int CHUNK_SIZE_Y = 256;
static constexpr int CHUNK_SIZE_Z = 16;
static constexpr int CHUNK_VOLUME = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;

static constexpr int CHUNK_SECTION_SIZE = 16;
static constexpr int CHUNK_SECTION_COUNT = CHUNK_SIZE_Y / CHUNK_SECTION_SIZE;
static constexpr int CHUNK_SECTION_VOLUME = CHUNK_SIZE_X * CHUNK_SECTION_SIZE * CHUNK_SIZE_Z;

using ChunkCoord = DirectX::XMINT3;

struct ChunkColumnCoord
{
	int cx = 0;
	int cz = 0;

	bool operator==(const ChunkColumnCoord& rhs) const
	{
		return cx == rhs.cx && cz == rhs.cz;
	}

	bool operator!=(const ChunkColumnCoord& rhs) const
	{
		return !(*this == rhs);
	}
};

struct BlockCell
{
	BLOCK_ID blockID = 0;
	STATE_INDEX stateIndex = 0;

	BlockCell& operator=(const BlockCell& other)
	{
		this->blockID = other.blockID;
		this->stateIndex = other.stateIndex;
		return *this;
	}

	bool operator==(const BlockCell& rhs) const
	{
		return this->blockID == rhs.blockID 
			&& this->stateIndex == rhs.stateIndex;
	}

	bool operator!=(const BlockCell& rhs) const
	{
		return !(*this == rhs);
	}

	const bool IsAir() const
	{
		return blockID == 0;
	}
};

struct ChunkSection
{
	std::array<BlockCell, CHUNK_SECTION_VOLUME> blocks{};
	bool dirty = true;
	bool dirtyQueued = false;

	ChunkSection()
	{
		blocks.fill(BlockCell{ 0, 0 });
	}
};

struct ChunkColumn
{
	ChunkColumnCoord coord;
	std::unique_ptr<ChunkSection> sections[CHUNK_SECTION_COUNT]{};

	std::array<uint64_t, CHUNK_SECTION_COUNT> renderObj;
};