#pragma once
#include "CModelDB.h"
#include "CRuntimeAtlas.h"

struct AtlasBuildJob
{
	uint64_t textureHash;
	string textureKey;
	string filePath;
};

class CRuntimeAtlasBuilder
{
public:
	CRuntimeAtlasBuilder() = default;
	~CRuntimeAtlasBuilder() = default;

	void Initialize(const char* resourceRoot, CModelDB* pModelDB, CRuntimeAtlas* pAtlas);
	void BeginCollect();
	bool StepBuild(ID3D11DeviceContext* pContext, uint32_t maxTilesPerStep);
	bool IsComplete() const;

private:
	string m_strResourceRoot;
	CModelDB* m_pModelDB = nullptr;
	CRuntimeAtlas* m_pAtlas = nullptr;

	vector<AtlasBuildJob> m_jobs;
	size_t m_uNextJobIndex = 0;
};