#pragma once
#include "CDeltaTimeManager.h"
#include "CSceneManager.h"

class CRenderWorld;

class CGameWorld
{
public:
	CGameWorld() = default;
	~CGameWorld() = default;

	void Initialize(CRenderWorld& renderWorld_);
	void Tick();
	void BuildRenderFrame();

private:
	void _RegisterScenes();

private:
	CDeltaTimeManager m_gameTimeManager;
	CSceneManager m_sceneManager;

	CRenderWorld* m_pRenderWorld = nullptr;

private: // fixed-update logic elements
	const float FIXED_DELTA = 1.0f / 60.0f; // 120.f는 추후
	const int MAX_FIXED_STEP = 5; // 5 * (FIXED_DELTA) = 약 83ms
	float m_fAccumulatedTime = 0.0f;
	int m_iFixedUpdateProcCnt = 0;
};