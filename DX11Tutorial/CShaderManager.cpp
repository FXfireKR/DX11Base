#include "pch.h"
#include "CShaderManager.h"

uint64_t CShaderManager::RegisterShaderName(const string& strName_)
{
	auto iter = m_mapShaderNameToID.find(strName_);
	if (iter == m_mapShaderNameToID.end()) {
		m_mapShaderNameToID.insert(make_pair(strName_, fnv1a_64(strName_)));
	}
	return m_mapShaderNameToID[strName_];
}

CShader* CShaderManager::GetShader(const ShaderKey& key_)
 {
	 auto iter = m_mapShaders.find(key_);
	 if (iter != m_mapShaders.end()) {
		 return iter->second.get();
	 }
	 return _CreateShader(key_);
 }

 uint64_t CShaderManager::GetShaderID(const string& strName_)
 {
	 auto iter = m_mapShaderNameToID.find(strName_);
	 
	 return 0;
 }

 CShader* CShaderManager::_CreateShader(const ShaderKey& key_)
 {
	 return nullptr;
 }
