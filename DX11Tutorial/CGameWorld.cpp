#include "pch.h"
#include "CGameWorld.h"

void CGameWorld::Initialize(CRenderWorld& renderWorld_)
{
	m_pRenderWorld = &renderWorld_;

	m_sceneManager.Initialize();
	
	m_sceneManager.Create(SCENE_TYPE::TEST_SCENE);
	m_sceneManager.ChangeScene(SCENE_TYPE::TEST_SCENE);
}

void CGameWorld::Tick()
{

}

void CGameWorld::BuildRenderFrame()
{

}