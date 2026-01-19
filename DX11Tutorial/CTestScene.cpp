#include "pch.h"
#include "CTestScene.h"

namespace // auto register
{
	bool registered = []() 
	{
		CSceneManager::Get().Register(SCENE_TYPE::TEST_SCENE, []() { return make_unique<CTestScene>(); });
		return true;
	}();
}

CTestScene::CTestScene()
{
}

CTestScene::~CTestScene()
{
}

void CTestScene::Awake()
{
}

void CTestScene::FixedUpdate()
{
}

void CTestScene::Update()
{
#ifdef IMGUI_ACTIVATE
	// ImGui
	{
		ImGui::Text("This is TestScene!");
	}
#endif // IMGUI_ACTIVATE
}

void CTestScene::LateUpdate()
{
}