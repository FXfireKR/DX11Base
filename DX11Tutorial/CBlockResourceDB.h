#pragma once
#include "singleton.h"

#include "CRuntimeAtlas.h"
#include "CRuntimeAtlasBuilder.h"

struct BlockCell;

class CBlockResourceDB : public singleton<CBlockResourceDB>
{
public:
	void Initialize(const char* resourceRoot, ID3D11Device* pDevice);
	bool Load();
	void Clear();

	bool RegisterTextureKey(const char* textureKey);
	bool RegisterTextureKeys(const vector<string>& keys);
	bool RegisterTextureKeys(const unordered_set<string>& keys);

	const AtlasRegion* FindAtlasRegion(const char* textureKey) const;
	bool TryGetRegion(const char* textureKey, AtlasRegion& outRegion) const;
	bool TryGetBlockParticleRegion(const char* textureKey, XMFLOAT2& outMinUV, XMFLOAT2& outMaxUV) const;

	inline ID3D11ShaderResourceView* GetAtlasTextureView() const { return m_runtimeAtlas.GetShaderResourceView(); }

private:
	bool _ResolveTextureFilePath(const char* textureKey, string& outPath) const;
	bool _BuildAtlasInputs(vector<AtlasBuildInput>& outInputs) const;


private:
	ID3D11Device* m_pDevice = nullptr;
	string m_strRoot;

	bool m_bInit = false;
	bool m_bLoadComplete = false;

	CRuntimeAtlasBuilder m_runtimeAtlasBuilder;
	CRuntimeAtlas m_runtimeAtlas;

	unordered_set<string> m_setTextureKeys;
};