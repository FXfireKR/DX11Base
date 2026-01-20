#pragma once
#include "CShader.h"

enum class SHADER_MACRO : uint32_t
{
	USE_INSTANCING = 1 << 0,
	USE_SKINNING = 1 << 1,
	USE_SHADOW = 1 << 2,
};

struct ShaderKey
{
	uint64_t uBaseShaderID = 0;
	uint32_t uMacroFlags = 0;

	bool operator==(const ShaderKey& other_) const {
		return (uBaseShaderID == other_.uBaseShaderID) && (uMacroFlags == other_.uMacroFlags);
	}
};

struct ShaderKeyHash
{
	size_t operator()(const ShaderKey& key_) const {
		size_t h1 = std::hash<uint64_t>{}(key_.uBaseShaderID);
		size_t h2 = std::hash<uint32_t>{}(key_.uMacroFlags);
		return h1 ^ (h2 << 1); // Combine hashes
	}
};

struct ShaderDesc
{
};

// TODO : 여기를 나중에 json으로 묶어서 로드하도록 바꾸면 좋을 것 같은데...

class CShaderManager
{
public:
	CShaderManager() = default;
	~CShaderManager() = default;

	uint64_t RegisterShaderName(const string& strName_);
	CShader* GetShader(const ShaderKey& key_);
	uint64_t GetShaderID(const string& strName_);

private:
	CShader* _CreateShader(const ShaderKey& key_);


private:
	unordered_map<string, uint64_t> m_mapShaderNameToID;
	unordered_map<uint64_t, ShaderDesc> m_mapShaderNameToID;
	unordered_map<ShaderKey, unique_ptr<CShader>, ShaderKeyHash> m_mapShaders;
};