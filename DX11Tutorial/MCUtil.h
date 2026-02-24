#pragma once
#include "MCModelUtl.h"
#include "MCStateUtil.h"

// Positive(+), Negative(-)
enum class FACE_DIR : uint8_t
{ 
	PX = 0, // East
	NX, // West
	PY, // Up
	NY, // Down
	PZ, // South
	NZ,  // North

	COUNT
};

static XMFLOAT3 FaceNormal(FACE_DIR eDir)
{
	switch (eDir)
	{
		case FACE_DIR::PX: return { 1.f, 0.f, 0.f };
		case FACE_DIR::NX: return { -1.f, 0.f, 0.f };
		case FACE_DIR::PY: return { 0.f, 1.f, 0.f };
		case FACE_DIR::NY: return { 0.f, -1.f, 0.f };
		case FACE_DIR::PZ: return { 0.f, 0.f, 1.f };
		case FACE_DIR::NZ: return { 0.f, 0.f, -1.f };
	}
}

static int ParseFaceDir(const char* strDir)
{
	// minecraft json: "down", "up", "north", "south", "west", "east"
	if (!strDir) return -1;
	if (strcmp(strDir, "east") == 0) return static_cast<int>(FACE_DIR::PX);
	if (strcmp(strDir, "west") == 0) return static_cast<int>(FACE_DIR::NX);
	if (strcmp(strDir, "up") == 0) return static_cast<int>(FACE_DIR::PY);
	if (strcmp(strDir, "down") == 0) return static_cast<int>(FACE_DIR::NY);
	if (strcmp(strDir, "south") == 0) return static_cast<int>(FACE_DIR::PZ);
	if (strcmp(strDir, "north") == 0) return static_cast<int>(FACE_DIR::NZ);
	return -1;
}

static uint8_t ParseAxis(const char* strAxis)
{
	// "x", "y", "z"
	if (!strAxis) return 0;
	if (strcmp(strAxis, "x") == 0) return 0;
	if (strcmp(strAxis, "y") == 0) return 1;
	if (strcmp(strAxis, "z") == 0) return 2;
	return 0;
} 

static float MCCoordTrans(float v) 
{
	return v / 16.0f; 
}

static void BuildFaceQuadPositions_01(const MCModelElement& modelElem, FACE_DIR eDir, XMFLOAT3 outPos01[4])
{
	const float x0 = MCCoordTrans(modelElem.from[0]);
	const float y0 = MCCoordTrans(modelElem.from[1]);
	const float z0 = MCCoordTrans(modelElem.from[2]);
	const float x1 = MCCoordTrans(modelElem.to[0]);
	const float y1 = MCCoordTrans(modelElem.to[1]);
	const float z1 = MCCoordTrans(modelElem.to[2]);

	switch (eDir)
	{
		case FACE_DIR::PX:
		{
			outPos01[0] = { x1, y0, z0 };
			outPos01[1] = { x1, y0, z1 };
			outPos01[2] = { x1, y1, z1 };
			outPos01[3] = { x1, y1, z0 };
		} break;
		case FACE_DIR::NX:
		{
			outPos01[0] = { x0, y0, z1 };
			outPos01[1] = { x0, y0, z0 };
			outPos01[2] = { x0, y1, z0 };
			outPos01[3] = { x0, y1, z1 };
		} break;
		case FACE_DIR::PY:
		{
			outPos01[0] = { x0, y1, z0 };
			outPos01[1] = { x1, y1, z0 };
			outPos01[2] = { x1, y1, z1 };
			outPos01[3] = { x0, y1, z1 };
		} break;
		case FACE_DIR::NY:
		{
			outPos01[0] = { x0, y0, z1 };
			outPos01[1] = { x1, y0, z1 };
			outPos01[2] = { x1, y0, z0 };
			outPos01[3] = { x0, y0, z0 };
		} break;
		case FACE_DIR::PZ:
		{
			outPos01[0] = { x1, y0, z1 };
			outPos01[1] = { x0, y0, z1 };
			outPos01[2] = { x0, y1, z1 };
			outPos01[3] = { x1, y1, z1 };
		} break;
		case FACE_DIR::NZ:
		{
			outPos01[0] = { x0, y0, z0 };
			outPos01[1] = { x1, y0, z0 };
			outPos01[2] = { x1, y1, z0 };
			outPos01[3] = { x0, y1, z0 };
		} break;
	}
}

static void ComputeFaceUV_Default(const MCModelElement& e, FACE_DIR eDir, float outUV01[4])
{
	float x0 = e.from[0];
	float y0 = e.from[1];
	float z0 = e.from[2];
	float x1 = e.to[0];
	float y1 = e.to[1];
	float z1 = e.to[2];

	switch (eDir)
	{
		case FACE_DIR::PX:
		case FACE_DIR::NX:
		{
			outUV01[0] = z0 / 16.f;
			outUV01[1] = y0 / 16.f;
			outUV01[2] = z1 / 16.f;
			outUV01[3] = y1 / 16.f;
		} break;

		case FACE_DIR::PY:
		case FACE_DIR::NY:
		{
			outUV01[0] = x0 / 16.f;
			outUV01[1] = z0 / 16.f;
			outUV01[2] = x1 / 16.f;
			outUV01[3] = z1 / 16.f;
		} break;

		case FACE_DIR::PZ:
		case FACE_DIR::NZ:
		{
			outUV01[0] = x0 / 16.f;
			outUV01[1] = y0 / 16.f;
			outUV01[2] = x1 / 16.f;
			outUV01[3] = y1 / 16.f;
		} break;
	}
}

static void ApplyUVRotation(float uv01[4], uint8_t rot)
{
	if (rot == 0)
		return;

	float u0 = uv01[0];
	float v0 = uv01[1];
	float u1 = uv01[2];
	float v1 = uv01[3];

	switch (rot)
	{
	case 90:
		// swap axes
		uv01[0] = u0;
		uv01[1] = v1;
		uv01[2] = u1;
		uv01[3] = v0;
		break;

	case 180:
		uv01[0] = u1;
		uv01[1] = v1;
		uv01[2] = u0;
		uv01[3] = v0;
		break;

	case 270:
		uv01[0] = u1;
		uv01[1] = v0;
		uv01[2] = u0;
		uv01[3] = v1;
		break;
	}
}

// block key 생성
static bool BuildBlockKeyFromPath(const filesystem::path& filePath, std::string& outBlockKey)
{
	std::string strPath = filePath.generic_string();
	
	size_t posAssets = strPath.find("/assets/");
	if (posAssets == std::string::npos) return false;

	size_t posBlockState = strPath.find("/blockstates/");
	if (posBlockState == std::string::npos) return false;

	size_t namespaceBeg = posAssets + std::string("/assets/").size();
	size_t namespaceEnd = posBlockState;
	if (namespaceEnd <= namespaceBeg) return false;
	std::string nameSpace = strPath.substr(namespaceBeg, namespaceEnd - namespaceBeg);

	size_t relBeg = posBlockState + std::string("/blockstates/").size();
	size_t relEnd = strPath.rfind(".json");
	if (relEnd == std::string::npos || relEnd <= relBeg) return false;
	std::string rel = strPath.substr(relBeg, relEnd - relBeg);

	outBlockKey = nameSpace + ":" + rel;
	return true;
}