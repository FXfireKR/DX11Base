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
	CRenderWorld* m_pRenderWorld = nullptr;
	
	CSceneManager m_sceneManager;
};