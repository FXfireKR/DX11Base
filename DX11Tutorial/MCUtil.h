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
		case FACE_DIR::PX: // EAST
		{
			outPos01[0] = { x1, y0, z1 };
			outPos01[1] = { x1, y0, z0 };
			outPos01[2] = { x1, y1, z0 };
			outPos01[3] = { x1, y1, z1 };
		} break;
		case FACE_DIR::NX: // WEST
		{
			outPos01[0] = { x0, y0, z0 };
			outPos01[1] = { x0, y0, z1 };
			outPos01[2] = { x0, y1, z1 };
			outPos01[3] = { x0, y1, z0 };
		} break;
		case FACE_DIR::PY: // UP
		{
			outPos01[0] = { x0, y1, z1 };
			outPos01[1] = { x1, y1, z1 };
			outPos01[2] = { x1, y1, z0 };
			outPos01[3] = { x0, y1, z0 };
		} break;
		case FACE_DIR::NY: // DOWN
		{
			outPos01[0] = { x0, y0, z0 };
			outPos01[1] = { x1, y0, z0 };
			outPos01[2] = { x1, y0, z1 };
			outPos01[3] = { x0, y0, z1 };
		} break;
		case FACE_DIR::PZ: // SOUTH
		{
			outPos01[0] = { x0, y0, z1 };
			outPos01[1] = { x1, y0, z1 };
			outPos01[2] = { x1, y1, z1 };
			outPos01[3] = { x0, y1, z1 };
		} break;
		case FACE_DIR::NZ: // NORTH
		{
			outPos01[0] = { x1, y0, z0 };
			outPos01[1] = { x0, y0, z0 };
			outPos01[2] = { x0, y1, z0 };
			outPos01[3] = { x1, y1, z0 };
		} break;
	}
}


static void ComputeFaceUV_Default(IN const MCModelElement& e, FACE_DIR eDir, OUT float uv01[4])
{
	const float x0 = MCCoordTrans(e.from[0]);
	const float y0 = MCCoordTrans(e.from[1]);
	const float z0 = MCCoordTrans(e.from[2]);
	const float x1 = MCCoordTrans(e.to[0]);
	const float y1 = MCCoordTrans(e.to[1]);
	const float z1 = MCCoordTrans(e.to[2]);

	float u0 = 0, v0 = 0, u1 = 1, v1 = 1;

	switch (eDir)
	{
		// side faces: U는 수평축, V는 Y(위아래)인데 텍스처는 top-left 기준이라 Y를 뒤집어야 “위쪽(y1)”이 “v0(0)”로 감
	case FACE_DIR::PZ: // south: u=x, v=y
	case FACE_DIR::NZ: // north: u=x, v=y (방향성은 positions에서 해결)
		u0 = x0; u1 = x1;
		v0 = 1.0f - y1; v1 = 1.0f - y0;
		break;

	case FACE_DIR::PX: // east: u=z, v=y
	case FACE_DIR::NX: // west: u=z, v=y
		u0 = z0; u1 = z1;
		v0 = 1.0f - y1; v1 = 1.0f - y0;
		break;

		// up/down: V축이 z로 간다. top-left 기준으로 “북쪽(-Z)이 위”가 되게 맞춘다.
	case FACE_DIR::PY: // up: u=x, v=z  (z0이 위쪽(v0))
		u0 = x0; u1 = x1;
		v0 = z0; v1 = z1;
		break;

	case FACE_DIR::NY: // down: u=x, v=z  (down은 “위쪽”이 반대로 느껴지기 쉬운데 positions에서 V축을 -Z로 잡았으므로 여기서는 동일 처리)
		u0 = x0; u1 = x1;
		v0 = z0; v1 = z1;
		break;

	default:
		break;
	}

	uv01[0] = u0; uv01[1] = v0;
	uv01[2] = u1; uv01[3] = v1;

}

static void ApplyUVRotation(XMFLOAT2 uv[4], uint8_t rotDeg)
{
	const uint8_t rot = (rotDeg % 360);
	if (rot == 0) return;

	XMFLOAT2 t[4];

	// uv order: [0]=LB, [1]=RB, [2]=RT, [3]=LT
	// CW 90: LB<-LT, RB<-LB, RT<-RB, LT<-RT
	switch (rot)
	{
	case 90:
		t[0] = uv[3]; // LB <- LT
		t[1] = uv[0]; // RB <- LB
		t[2] = uv[1]; // RT <- RB
		t[3] = uv[2]; // LT <- RT
		break;
	case 180:
		t[0] = uv[2];
		t[1] = uv[3];
		t[2] = uv[0];
		t[3] = uv[1];
		break;
	case 270:
		t[0] = uv[1];
		t[1] = uv[2];
		t[2] = uv[3];
		t[3] = uv[0];
		break;
	default:
		return;
	}

	memcpy(uv, t, sizeof(t));
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