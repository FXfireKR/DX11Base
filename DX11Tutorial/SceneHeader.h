#pragma once

#include "CTestScene.h"

enum class SCENE_TYPE : unsigned char
{
	TEST_SCENE = 0,

	//Add Scene here...
	END_SCENE,
};

constexpr uint32_t SCENE_MAX_SIZE = static_cast<uint32_t>(SCENE_TYPE::END_SCENE);