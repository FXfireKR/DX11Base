#pragma once
#include "singleton.h"

using MODEL_ID = uint32_t;

struct ModelEntry
{
	string key;
	bool bRawLoaded = false;
	MCModelRaw raw;

	bool bResolved = false;
	MCModelResolved resolved;

	bool bBaked = false;
	BakedModel baked;
};

class CModelDB : public singleton<CModelDB>
{
public:
	void Initialize(const string& resourceRoot);
	
	MODEL_ID LoadModel(const char* modelKey);
	MODEL_ID GetModel(uint64_t modelHash);
	MODEL_ID GetModel(const char* modelKey);

	const BakedModel* GetBaked(MODEL_ID id) const;

private:
	bool _LoadRawModelJSON(IN const char* modelKey, OUT MCModelRaw& modelRaw);
	bool _ResolveModel(IN const char* modelKey, OUT MCModelResolved& modelResolved);
	bool _ResolveParentRecursive(IN const char* modelKey, MCModelResolved& modelResolved, unordered_set<string>& visitedStack);

	string _ResolveTextureRef(const MCModelResolved& model, const string& texRef, int depth = 0);
	void _BakeElements(IN const MCModelResolved& modelResolved, OUT BakedModel& bakedModel);
	void _BakeOneElementFace(IN const MCModelResolved& modelResolved, const MCModelElement& modelElem, int faceDir, OUT BakedModel& bakedModel);

	string _BuildModelPath(string resourceRoot, string modelKey);

private:
	string m_strResourceRoot;
	vector<unique_ptr<ModelEntry>> m_vecEntries;
	unordered_map<uint64_t, MODEL_ID> m_mapKeyToID;
};