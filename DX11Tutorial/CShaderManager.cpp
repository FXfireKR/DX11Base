#include "pch.h"
#include "CShaderManager.h"

void CShaderManager::Initialize()
{
	_LoadShaderDescs();
}

void CShaderManager::CreateShader(uint64_t uShaderID_, uint32_t uShaderMacroFlags_)
{
	
}

void CShaderManager::CreateShaderByName(const string& strName_, uint32_t uShaderMacroFlags_)
{

}

const CShader* CShaderManager::GetShader(uint64_t uShaderID_, uint32_t uShaderMacroFlags_) const
{
	ShaderKey toFindShaderKey {uShaderID_, uShaderMacroFlags_};
	auto iterShader = m_mapShaders.find(toFindShaderKey);
	if (iterShader != m_mapShaders.end()) {
		return iterShader->second.get();
	}
	return nullptr;
}

const CShader* CShaderManager::GetShaderByName(const string& strName_, uint32_t uShaderMacroFlags_) const
{
	auto shaderIDPair = m_mapShaderNameToID.find(strName_);
	if (shaderIDPair != m_mapShaderNameToID.end()) {
		return GetShader(shaderIDPair->second, uShaderMacroFlags_);
	}
	return nullptr;
}

bool CShaderManager::GetShaderDesc(__in uint64_t uShaderID_, __out SHADER_DESC& refShaderDesc_)
{
	refShaderDesc_ = SHADER_DESC{};

	auto iterShaderDesc = m_mapShaderDesc.find(uShaderID_);
	if (iterShaderDesc != m_mapShaderDesc.end()) {
		refShaderDesc_ = iterShaderDesc->second;
		return true;
	}
	return false;
}

bool CShaderManager::GetShaderDescByName(__in const string& strName_, __out SHADER_DESC& refShaderDesc_)
{
	auto shaderIDPair = m_mapShaderNameToID.find(strName_);
	if (shaderIDPair != m_mapShaderNameToID.end()) {
		return GetShaderDesc(shaderIDPair->second, refShaderDesc_);
	}

	refShaderDesc_ = SHADER_DESC{};
	return false;
}

 void CShaderManager::_LoadShaderDescs()
 {
	 // 하드 경로는 나중에 ini로 빼자
	 filesystem::path shaderDataPath = std::filesystem::current_path().parent_path();
	 shaderDataPath = shaderDataPath / "Shader/shaders.json";

	 ifstream jsonFile(shaderDataPath.string(), std::ios::binary);
	 if (false == jsonFile.is_open()) return;

	 std::stringstream jsonBuffer;
	 jsonBuffer << jsonFile.rdbuf();

	 Document docs;
	 // available comments & Trailing commas
	 docs.Parse<kParseCommentsFlag|kParseTrailingCommasFlag>(jsonBuffer.str().c_str());

	 // close file when end parsing
	 jsonFile.close();

	 if (docs.HasParseError()) {
		 ParseErrorCode ecode = docs.GetParseError();
		 cout << "Shaders.json parsing error!";
		 return;
	 }

	 // if it was not OBJECT eject
	 if (!docs.IsObject()) return;

	 for (auto& shaderMember : docs.GetObj()) {
		 string strShaderName(shaderMember.name.GetString());
		 const Value& shaderObject = shaderMember.value;

		 if (!shaderObject.IsObject()) {
			 cout << strShaderName.c_str() << " is not JSON Object!" << endl;
			 continue;
		 }

		 uint64_t uShaderID = fnv1a_64(strShaderName);
		 if (m_mapShaderNameToID.contains(strShaderName)) {
			 cout << strShaderName.c_str() << " is already Exist in Shader-map!" << endl;
			 continue;
		 }

		 m_mapShaderNameToID.insert(make_pair(strShaderName, uShaderID));
		 SHADER_DESC newShaderDESC;

		 // read VS
		 if (true == shaderObject.HasMember("VS")) {
			 const Value& vs = shaderObject["VS"];
			 if (true == vs.IsObject()) {
				 newShaderDESC.strVertexShaderPath = string(vs["path"].GetString());
				 newShaderDESC.strVertexShaderEntry = string(vs["entry"].GetString());
				 newShaderDESC.strVertexShaderVersion = string(vs["version"].GetString());
			 }
		 }

		 // read PS
		 if (true == shaderObject.HasMember("PS")) {
			 const Value& ps = shaderObject["PS"];
			 if (true == ps.IsObject()) {
				 newShaderDESC.strPixelShaderPath = string(ps["path"].GetString());
				 newShaderDESC.strPixelShaderEntry = string(ps["entry"].GetString());
				 newShaderDESC.strPixelShaderVersion = string(ps["version"].GetString());
			 }
		 }
		 m_mapShaderDesc.insert(make_pair(uShaderID, newShaderDESC));
	 }
 }
