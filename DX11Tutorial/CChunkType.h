#pragma once
using BLOCK_ID = uint16_t;

constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 16;
constexpr int CHUNK_SIZE_Z = 16;
constexpr int CHUNK_VOLUME = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;

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