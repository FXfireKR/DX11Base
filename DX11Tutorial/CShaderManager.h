#pragma once
#include "CShader.h"

/*	[ Shader life-time ]
*	- Never release during runtime.
*	- Released only at engine shutdown.
*/

constexpr const char* ERROR_SHADER_NAME = "Error";

enum class SHADER_MACRO : uint32_t
{
	USE_INSTANCING = 1 << 0,
	USE_SKINNING = 1 << 1,
	USE_SHADOW = 1 << 2,
	// 여기 추가되면 _ConvertFlagToShaderMacro 함수에도 추가해야함.
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
	void Compile();

	const CShader* CreateShader(uint64_t uShaderID_, uint32_t uShaderMacroFlags_);
	const CShader* CreateShaderByName(const string& strName_, uint32_t uShaderMacroFlags_);

	const CShader* GetShader(uint64_t uShaderID_, uint32_t uShaderMacroFlags_) const;
	const CShader* GetShaderByName(const string& strName_, uint32_t uShaderMacroFlags_) const;

	const CShader* GetErrorShader() const;

	bool GetShaderDesc(__in uint64_t uShaderID_, __out SHADER_DESC& refShaderDesc_);
	bool GetShaderDescByName(__in const string& strName_, __out SHADER_DESC& refShaderDesc_);

	void IsCompileDone() const;
public:
	inline size_t GetCurrentCompileWait() { return m_queCompileWait.size(); }
	inline void SetMaxShaderCompileCount(uint32_t uCount) { m_uMaxShaderCompileCount = max(uCount, 1); }

private:
	void _LoadShaderDescs();
	void _LoadErrorFallbackShader();
	vector<D3D_SHADER_MACRO> _ConvertFlagToShaderMacro(uint32_t uMacroFlags_);

private:
	unordered_map<string, uint64_t> m_mapShaderNameToID;
	unordered_map<uint64_t, SHADER_DESC> m_mapShaderDesc;
	unordered_map<ShaderKey, unique_ptr<CShader>, ShaderKeyHash> m_mapShaders;

	// shader compile queue
	queue<ShaderKey> m_queCompileWait;
	uint32_t m_uMaxShaderCompileCount = 1; // 한번에 컴파일 할 쉐이더 최대 갯수

	uint32_t m_uErrorShaderID = UINT_MAX;
};