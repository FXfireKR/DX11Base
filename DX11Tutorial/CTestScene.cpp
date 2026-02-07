#include "pch.h"
#include "CTestScene.h"

CTestScene::CTestScene()
{
}

CTestScene::~CTestScene()
{
}

void CTestScene::Awake()
{
}

void CTestScene::FixedUpdate(float fDelta)
{
}

void CTestScene::Update(float fDelta)
{
#ifdef IMGUI_ACTIVATE
	// ImGui
	{
		ImGui::Text("This is TestScene!");
	}
#endif // IMGUI_ACTIVATE
}

void CTestScene::LateUpdate(float fDelta)
{
}