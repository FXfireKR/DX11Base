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

class CShaderManager : public singleton<CShaderManager>
{
public:
	CShaderManager() = default;
	~CShaderManager() = default;

	void Initialize();
	void BeginCompile();

	void CreateShader(uint64_t uShaderID_, uint32_t uShaderMacroFlags_);
	void CreateShaderByName(const string& strName_, uint32_t uShaderMacroFlags_);

	const CShader* GetShader(uint64_t uShaderID_, uint32_t uShaderMacroFlags_) const;
	const CShader* GetShaderByName(const string& strName_, uint32_t uShaderMacroFlags_) const;

	bool GetShaderDesc(__in uint64_t uShaderID_, __out SHADER_DESC& refShaderDesc_);
	bool GetShaderDescByName(__in const string& strName_, __out SHADER_DESC& refShaderDesc_);
	
	
	bool IsCompiled();

private:
	void _LoadShaderDescs();

private:
	unordered_map<string, uint64_t> m_mapShaderNameToID; // 1:1 name-id
	unordered_map<uint64_t, SHADER_DESC> m_mapShaderDesc; // 1:1 id-desc
	unordered_map<ShaderKey, unique_ptr<CShader>, ShaderKeyHash> m_mapShaders;
};