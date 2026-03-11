#pragma once
#include "VoxelTypes.h"

#include <string>
#include <vector>

enum class BLOCK_RENDER_LAYER : uint8_t
{
	OPAQUE_LAYER = 0,
	CUTOUT_LAYER,
	TRANSLUCENT_LAYER,
	INVISIBLE_LAYER,
};

enum class BLOCK_COLLISION_TYPE : uint8_t
{
	NONE = 0,
	FULL_CUBE,
	CUSTOM,
};

struct BlockPropertyFlags
{
	bool bAir = false;
	bool bOpaque = false;
	bool bSolid = false;
	bool bFullCube = false;
};

struct BlockRenderDef
{
	BLOCK_RENDER_LAYER layer = BLOCK_RENDER_LAYER::OPAQUE_LAYER;
	bool bAmbientOcclusion = true;
};

struct BlockCollisionDef
{
	BLOCK_COLLISION_TYPE colType = BLOCK_COLLISION_TYPE::NONE;
};

//struct BlockDef
//{
//	BLOCK_ID blockID = INVALID_BLOCK_ID;
//	
//	std::string name;			// minecraft:stone
//	std::string parent;			// optional template parent
//	std::string stateSource;	// "stone"
//	std::string soundProfile;	// "stone"
//
//	BlockPropertyFlags properties;
//	BlockRenderDef render;
//	BlockCollisionDef collision;
//	std::vector<std::string> tags;
//
//	bool bIsTemplate = false;
//	bool bLoaded = false;
//};

struct BlockDefRaw
{
	std::string name;
	std::string parent;
	rapidjson::Document doc;
};

struct BlockDef
{
	std::string name;
	std::string parent;
	
	BlockPropertyFlags properties;

	std::string model;
	std::vector<std::string> tags;
};