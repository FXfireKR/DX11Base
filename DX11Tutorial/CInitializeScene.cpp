#include "pch.h"
#include "CInitializeScene.h"
#include "CRenderWorld.h"

void CInitializeScene::Awake()
{
	CRenderWorld& rw = GetRenderWorld();

	BlockDB.Initialize("../Resource/");
	BlockResDB.Initialize("../Resource/", rw.GetDevice());
}

void CInitializeScene::FixedUpdate(float fDelta)
{
	UNREFERENCED_PARAMETER(fDelta);
}

void CInitializeScene::Update(float fDelta)
{
	CScene::Update(fDelta);

	BlockDB.Load();

	if (BlockDB.IsLoadedComplete())
	{
		BlockResDB.RegisterTextureKeys(BlockDB.GetUsedTextureKeys());
		BlockResDB.Load();

		m_bChangeSceneReq = true;
		m_eNextSceneReq = SCENE_TYPE::TEST_SCENE;
	}

//#ifdef IMGUI_ACTIVATE
//	ImGui::Text("This is Initialize Scne!");
//
//	ImGui::Text("max files : %u", CBlockStateDB::Get().GetMaxLoadedCnt());
//	ImGui::Text("loaded files : %u", CBlockStateDB::Get().GetCurrentLoadedCnt());
//#endif // IMGUI_ACTIVATE
//
//	if (CBlockStateDB::Get().IsLoadComplete())
//	{
//		m_bChangeSceneReq = true;
//		m_eNextSceneReq = SCENE_TYPE::TEST_SCENE;
//	}
}

void CInitializeScene::LateUpdate(float fDelta)
{
	UNREFERENCED_PARAMETER(fDelta);
}