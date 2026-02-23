#pragma once

// elements
struct MCModelFace
{
	string textureRef;

	bool bHasUV = false;
	float uv[4] = { 0.f, 0.f, 0.f, 0.f };

	bool bHasCull = false;
	uint8_t cullDir = 0;

	uint8_t rotation = 0;

	int tintIndex = -1;
};

struct MCModelElement
{
	// from/to : 0~16 in units
	float from[3] = { 0.f, 0.f, 0.f };
	float to[3] = { 16.f, 16.f, 16.f };

	bool bHasRotation = false;
	float rotOrigin[3] = { 8.f, 8.f, 8.f };
	uint8_t rotAxis = 0; // X / Y / Z
	float rotAngleDeg = 0.0f;
	bool bRescale = false;

	// face
	bool bHasFace[6] = { false, false, false, false, false, false, };
	MCModelFace face[6];
};

struct MCModelRaw
{
	// parent: "minecraft:block/cube_all"
	bool bHasParent = false;
	string parentKey;

	// textures: { "all" : "minecraft:block/stone", ... }
	unordered_map<string, string> textures;

	vector<MCModelElement> elements;
};

struct MCModelResolved
{
	// textures: key->Value
	unordered_map<string, string> textures;
	vector<MCModelElement> elements;
};

struct BakedVertex
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
	uint32_t color; // tint용
};

struct BakedQuad
{
	BakedVertex vert[4];
	uint32_t textureID;
	uint8_t cullDir;
};

struct BakedModel
{
	vector<BakedQuad> quads;
};

struct CPUMeshData
{
	vector<BakedVertex> vertices;
	vector<uint32_t> indices;
};