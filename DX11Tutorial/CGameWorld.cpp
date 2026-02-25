#include "pch.h"
#include "CGameWorld.h"
#include "CRenderWorld.h"

void CGameWorld::Initialize(CRenderWorld& renderWorld_)
{
	m_pRenderWorld = &renderWorld_;

	m_gameTimeManager.Init();

	_RegisterScenes();
	m_sceneManager.Initialize(*this);
	
	uint8_t it = static_cast<uint8_t>(SCENE_TYPE::BOOT_SCENE) + 1;
	for (; it < static_cast<uint8_t>(SCENE_TYPE::END_SCENE); ++it) {
		m_sceneManager.Create(static_cast<SCENE_TYPE>(it));
	}
	
	m_sceneManager.ChangeScene(SCENE_TYPE::INIT_SCENE);
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

	m_sceneManager.CheckChangeScene();
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
	m_sceneManager.Register(SCENE_TYPE::INIT_SCENE, []() { return make_unique<CInitializeScene>(); });
	m_sceneManager.Register(SCENE_TYPE::TEST_SCENE, []() { return make_unique<CTestScene>(); });
}