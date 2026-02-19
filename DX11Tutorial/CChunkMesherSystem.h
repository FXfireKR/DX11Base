#pragma once

class CScene;

class CChunkMesherSystem
{
public:
	static void RebuildDirtyChunks(CScene& scene);
};