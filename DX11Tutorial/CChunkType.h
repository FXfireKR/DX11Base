#pragma once

constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 256;
constexpr int CHUNK_SIZE_Z = 16;
constexpr int CHUNK_VOLUME = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;

constexpr int CHUNK_SECTION_SIZE = 16;
constexpr int CHUNK_SECTION_COUNT = CHUNK_SIZE_Y / CHUNK_SECTION_SIZE;

struct ChunkCoord
{
	int x = 0;
	int y = 0;
	int z = 0;
};

inline int GetChunkIndex(int x, int y, int z)
{
	return x + (y * CHUNK_SIZE_X) + (z * CHUNK_SIZE_X * CHUNK_SIZE_Y);
}

inline bool InChunk(int x, int y, int z)
{
	return (0 <= x && x < CHUNK_SIZE_X) &&
		(0 <= y && y < CHUNK_SIZE_Y) &&
		(0 <= z && z < CHUNK_SIZE_Z);
}

using BLOCK_ID = uint32_t;