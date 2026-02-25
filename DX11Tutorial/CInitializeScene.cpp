#include "pch.h"
#include "CInitializeScene.h"

void CInitializeScene::Awake()
{
	CModelDB::Get().Initialize("../Resource/");
	CBlockStateDB::Get().Initialize("../Resource/");
}

void CInitializeScene::FixedUpdate(float fDelta)
{
	UNREFERENCED_PARAMETER(fDelta);
}

void CInitializeScene::Update(float fDelta)
{
	CScene::Update(fDelta);

	CBlockStateDB::Get().Load();

#ifdef IMGUI_ACTIVATE
	ImGui::Text("This is Initialize Scne!");

	ImGui::Text("max files : %u", CBlockStateDB::Get().GetMaxLoadedCnt());
	ImGui::Text("loaded files : %u", CBlockStateDB::Get().GetCurrentLoadedCnt());
#endif // IMGUI_ACTIVATE

	if (CBlockStateDB::Get().IsLoadComplete())
	{
		m_bChangeSceneReq = true;
		m_eNextSceneReq = SCENE_TYPE::TEST_SCENE;
	}
}

void CInitializeScene::LateUpdate(float fDelta)
{
	UNREFERENCED_PARAMETER(fDelta);
}