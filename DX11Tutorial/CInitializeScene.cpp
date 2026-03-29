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

		auto& preloadQueue = BlockResDB.GetPreLoadQueue();
		while(!preloadQueue.empty())
		{
			const auto& elem = preloadQueue.front();
			GetAudioSystem().LoadSound(elem.id, elem.objectPath.c_str(), elem.desc.b3D, elem.desc.bLoop);
			preloadQueue.pop();
		}

		m_bChangeSceneReq = true;
		m_eNextSceneReq = SCENE_TYPE::GAME_SCENE;
	}
}

void CInitializeScene::LateUpdate(float fDelta)
{
	UNREFERENCED_PARAMETER(fDelta);
}