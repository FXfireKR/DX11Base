#include "pch.h"
#include "CSceneManager.h"

void CSceneManager::Initialize()
{
	Create(SCENE_TYPE::BOOT_SCENE);
	m_eCurrentSceneType = SCENE_TYPE::BOOT_SCENE;

	CScene* pCurrentScene = _GetCurrent();
	pCurrentScene->Awake();
}

void CSceneManager::Register(SCENE_TYPE eType_, SceneFactory factory_)
{
	m_mapFactories[eType_] = move(factory_);
}

void CSceneManager::Create(SCENE_TYPE eType_)
{
	auto iter = m_mapFactories.find(eType_);
	if (iter != m_mapFactories.end()) {
		uint32_t uIndex = static_cast<uint32_t>(eType_);
		m_arrayScene[uIndex] = iter->second();

		m_arrayScene[uIndex]->OnCreate();
	}
}

void CSceneManager::Remove(SCENE_TYPE eType_)
{
}

void CSceneManager::ChangeScene(SCENE_TYPE eNext_)
{
	if (m_eSceneChangeState != SCENE_CHANGE_STATE::NONE) return;
	if (eNext_ == SCENE_TYPE::END_SCENE) return;

	m_eNextSceneType = eNext_;
	m_eSceneChangeState = SCENE_CHANGE_STATE::UNLOADING;
}



void CSceneManager::Update()
{
	_FixedUpdate();
	_Update();
	_LateUpdate();
}

void CSceneManager::_FixedUpdate()
{
	_GetCurrent()->FixedUpdate();
}

void CSceneManager::_Update()
{
	_GetCurrent()->Update();
}

void CSceneManager::_LateUpdate()
{
	switch (m_eSceneChangeState)
	{
		case SCENE_CHANGE_STATE::NONE:
		{
			_GetCurrent()->LateUpdate();
		} break;

		case SCENE_CHANGE_STATE::UNLOADING: 
		{
			_UnloadCurrentScene();
			m_eSceneChangeState = SCENE_CHANGE_STATE::LOADING;
		} break;

		case SCENE_CHANGE_STATE::LOADING: 
		{
			_LoadNextScene();
			m_eSceneChangeState = SCENE_CHANGE_STATE::ACTIVATING;
		} break;

		case SCENE_CHANGE_STATE::ACTIVATING:
		{
			_ActivatingCurrentScene();
			m_eSceneChangeState = SCENE_CHANGE_STATE::NONE;
		} break;
	}
}

void CSceneManager::_UnloadCurrentScene()
{
	if (m_eCurrentSceneType == SCENE_TYPE::END_SCENE) return;

}

void CSceneManager::_LoadNextScene()
{
}

void CSceneManager::_ActivatingCurrentScene()
{
	_GetNext()->Awake();
	m_eCurrentSceneType = m_eNextSceneType;
}