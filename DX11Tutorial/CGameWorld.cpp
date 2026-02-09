#include "pch.h"
#include "CGameWorld.h"
#include "CRenderWorld.h"

void CGameWorld::Initialize(CRenderWorld& renderWorld_)
{
	m_pRenderWorld = &renderWorld_;

	m_gameTimeManager.Init();

	_RegisterScenes();
	m_sceneManager.Initialize(*this);
	
	m_sceneManager.Create(SCENE_TYPE::TEST_SCENE);
	m_sceneManager.ChangeScene(SCENE_TYPE::TEST_SCENE);
}

void CGameWorld::Tick()
{
	m_gameTimeManager.Tick();
	float fDelta = m_gameTimeManager.GetDeltaTime();

	ImGui::Text("FPS : %u", m_gameTimeManager.GetFps());

	m_fAccumulatedTime += fDelta;
	m_iFixedUpdateProcCnt = 0;

	// fixed-update logic
	while (m_fAccumulatedTime >= FIXED_DELTA && m_iFixedUpdateProcCnt < MAX_FIXED_STEP) {
		m_sceneManager.FixedUpdate(FIXED_DELTA);
		m_fAccumulatedTime -= FIXED_DELTA;
		++m_iFixedUpdateProcCnt;
	}

	m_sceneManager.Update(fDelta);

	m_sceneManager.LateUpdate(fDelta);

	// pedding destroy
}

void CGameWorld::BuildRenderFrame()
{
	m_pRenderWorld->BeginBuildFrame();
	{
		m_sceneManager.BuildRenderFrame();
	}
	m_pRenderWorld->EndBuildFrame();
}

void CGameWorld::_RegisterScenes()
{
	m_sceneManager.Register(SCENE_TYPE::BOOT_SCENE, []() { return make_unique<CBootScene>(); });
	m_sceneManager.Register(SCENE_TYPE::TEST_SCENE, []() { return make_unique<CTestScene>(); });
}