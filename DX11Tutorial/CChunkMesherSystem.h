#pragma once

class CScene;
class CChunkWorld;

class CChunkMesherSystem
{
public:
	static void RebuildDirtyChunks(CScene& scene, CChunkWorld& world);
};