#include "pch.h"
#include "CBlockResourceDB.h"

void CBlockResourceDB::Initialize(const char* resourceRoot, ID3D11Device* pDevice)
{
	m_strRoot = resourceRoot ? resourceRoot : "";
	m_pDevice = pDevice;

	m_bInit = true;
	m_bLoadComplete = false;

	m_runtimeAtlasBuilder.Clear();
	m_runtimeAtlasBuilder.SetPadding(1);
	m_runtimeAtlasBuilder.SetDefaultTileSize(16);
}

bool CBlockResourceDB::Load()
{
	if (!m_bLoadComplete)
	{
		vector<AtlasBuildInput> inputs;
		if (!_BuildAtlasInputs(inputs))
			return false;

		m_runtimeAtlasBuilder.AddInputs(inputs);
		if (!m_runtimeAtlasBuilder.Build(m_pDevice, m_runtimeAtlas))
			return false;

		m_bLoadComplete = true;
	}
	return true;
}

void CBlockResourceDB::Clear()
{
	m_runtimeAtlasBuilder.Clear();
	m_runtimeAtlas.Clear();
}

bool CBlockResourceDB::RegisterTextureKey(const char* textureKey)
{
	if (nullptr == textureKey || textureKey[0] == '\0')
		return false;

	m_setTextureKeys.emplace(textureKey);
	return true;
}

bool CBlockResourceDB::RegisterTextureKeys(const vector<string>& keys)
{
	bool bAllSuccess = true;
	for (const string& key : keys)
	{
		if (!RegisterTextureKey(key.c_str()))
			bAllSuccess = false;
	}
	return bAllSuccess;
}

bool CBlockResourceDB::RegisterTextureKeys(const unordered_set<string>& keys)
{
	m_setTextureKeys = keys;
	return true;
}

const AtlasRegion* CBlockResourceDB::FindAtlasRegion(const char* textureKey) const
{
	return m_runtimeAtlas.FindRegion(textureKey);
}

bool CBlockResourceDB::TryGetRegion(const char* textureKey, AtlasRegion& outRegion) const
{
	return m_runtimeAtlas.TryGetRegion(textureKey, outRegion);
}

bool CBlockResourceDB::_ResolveTextureFilePath(const char* textureKey, string& outPath) const
{
	outPath.clear();

	if (textureKey == nullptr || textureKey[0] == '\0')
		return false;

	std::string key = textureKey;

	// "minecraft:block/stone"
	constexpr const char* prefix = "minecraft:";
	if (key.rfind(prefix, 0) != 0)
		return false;

	std::string relative = key.substr(strlen(prefix)); // "block/stone"
	outPath = m_strRoot + "assets/minecraft/textures/" + relative + ".png";
	return true;
}

bool CBlockResourceDB::_BuildAtlasInputs(vector<AtlasBuildInput>& outInputs) const
{
	outInputs.clear();
	outInputs.reserve(m_setTextureKeys.size());

	for (const string& key : m_setTextureKeys)
	{
		string filePath;
		if (!_ResolveTextureFilePath(key.c_str(), filePath))
			return false;

		AtlasBuildInput input;
		input.textureKey = key;
		input.filePath = filePath;

		if (!filesystem::exists(filePath))
		{
#ifdef _DEBUG
			cout << "CBlockResourceDB::_BuildAtlasInputs] " << filePath << " was not valid file!" << endl;
#endif // _DEBUG
			return false;
		}

		outInputs.push_back(std::move(input));
	}

	return !outInputs.empty();
}