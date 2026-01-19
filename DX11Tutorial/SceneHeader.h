#pragma once

#include "CTestScene.h"

enum class SCENE_TYPE : unsigned char
{
	BOOT_SCENE = 0,
	TEST_SCENE,

	//Add Scene here...
	END_SCENE,
};

constexpr uint32_t SCENE_MAX_SIZE = static_cast<uint32_t>(SCENE_TYPE::END_SCENE);