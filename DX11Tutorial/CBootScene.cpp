#include "pch.h"
#include "CBootScene.h"

namespace // auto register
{
	bool registered = []() {
		CSceneManager::Get().Register(SCENE_TYPE::BOOT_SCENE, []() { 
			return make_unique<CBootScene>();
		});

		return true;
	}();
}

CBootScene::CBootScene()
{
}

CBootScene::~CBootScene()
{
}

void CBootScene::Awake()
{
}

void CBootScene::FixedUpdate()
{
}

void CBootScene::Update()
{
}

void CBootScene::LateUpdate()
{
}