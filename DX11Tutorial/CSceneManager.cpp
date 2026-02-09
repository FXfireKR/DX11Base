#include "pch.h"
#include "CSceneManager.h"

void CSceneManager::Initialize(CGameWorld& gameWorld)
{
	m_pGameWorld = &gameWorld;

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

		m_arrayScene[uIndex]->OnCreate(*m_pGameWorld);
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

void CSceneManager::FixedUpdate(float fDelta)
{
	_FixedUpdate(fDelta);
}

void CSceneManager::Update(float fDelta)
{
	_Update(fDelta);
}

void CSceneManager::LateUpdate(float fDelta)
{
	_LateUpdate(fDelta);
}

void CSceneManager::BuildRenderFrame()
{
	_GetCurrent()->BuildRenderFrame();
}

void CSceneManager::_FixedUpdate(float fDelta)
{
	_GetCurrent()->FixedUpdate(fDelta);
}

void CSceneManager::_Update(float fDelta)
{
	_GetCurrent()->Update(fDelta);
}

void CSceneManager::_LateUpdate(float fDelta)
{
	switch (m_eSceneChangeState)
	{
		case SCENE_CHANGE_STATE::NONE:
		{
			_GetCurrent()->LateUpdate(fDelta);
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
	_GetNext()->Awake();
}

void CSceneManager::_ActivatingCurrentScene()
{
	_GetNext()->Start();
	m_eCurrentSceneType = m_eNextSceneType;
}