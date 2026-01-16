#include "pch.h"
#include "CSceneManager.h"

CSceneManager::CSceneManager()
{
}

CSceneManager::~CSceneManager()
{
}

void CSceneManager::Register(SCENE_TYPE eType_, SceneFactory factory_)
{
	m_mapFactories[eType_] = move(factory_);
}

void CSceneManager::Create(SCENE_TYPE eType_)
{
	auto iter = m_mapFactories.find(eType_);
	if (iter != m_mapFactories.end()) {
		m_arrayScene[static_cast<uint32_t>(eType_)] = iter->second();
	}
}

void CSceneManager::Remove(SCENE_TYPE eType_)
{
	
}