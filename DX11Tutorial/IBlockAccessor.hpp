#pragma once

struct Int3 { int x, y, z; };

struct BlockHitResult
{
	bool bHit = false;
	Int3 block{};	// 맞은 블록 좌표
	Int3 prev{};	// 바로 직전 좌표
	Int3 normal{};	// 맞은 면 노멀
	float t = 0.f;	// ray Parameter (대략적 거리)
};

// Accessor
class IBlockAccessor
{
public:
	virtual ~IBlockAccessor() = default;
	virtual BLOCK_ID GetBlock(int x, int y, int z) const = 0;
	virtual bool IsSolid(BLOCK_ID id) const = 0;
};

// 임시
class CTestBlockAccessor : public IBlockAccessor
{
public:
	~CTestBlockAccessor() override = default;

	BLOCK_ID GetBlock(int x, int y, int z) const override
	{
		if (x == m_pos.x && y == m_pos.y && z == m_pos.z) return m_blockID;
		return 0;
	}

	bool IsSolid(BLOCK_ID id) const override
	{
		return id != 0;
	}

private:
	BLOCK_ID m_blockID = 1;
	Int3 m_pos{ 0,0,0 };

};

static float SafeInv(float v)
{
	const float eps = 1e-8f; // custom epsilon
	if (fabs(v) < eps) return numeric_limits<float>::infinity();
	return 1.0f / v;
}

static bool RaycastVoxelDDA(IN const IBlockAccessor& world, const XMFLOAT3& origin, const XMFLOAT3& dirNorm, float maxDist, OUT BlockHitResult& hitResult)
{
	XMVECTOR O = XMLoadFloat3(&origin);
	XMVECTOR D = XMLoadFloat3(&dirNorm);

	int x = static_cast<int>(floor(origin.x));
	int y = static_cast<int>(floor(origin.y));
	int z = static_cast<int>(floor(origin.z));

	int stepX = (dirNorm.x > 0.f) ? 1 : ((dirNorm.x < 0.f) ? -1 : 0);
	int stepY = (dirNorm.y > 0.f) ? 1 : ((dirNorm.y < 0.f) ? -1 : 0);
	int stepZ = (dirNorm.z > 0.f) ? 1 : ((dirNorm.z < 0.f) ? -1 : 0);

	float invDx = SafeInv(dirNorm.x);
	float invDy = SafeInv(dirNorm.y);
	float invDz = SafeInv(dirNorm.z);

    // 다음 경계면까지의 t (tMax)와, 한 셀 넘어갈 때 증가량 (tDelta)
    auto nextBoundary = [](float o, int cell, int step) -> float
    {
        // step>0 이면 다음 정수 경계(cell+1), step<0 이면 현재 정수 경계(cell)
        if (step > 0) return (float)(cell + 1);
        if (step < 0) return (float)(cell);
        return std::numeric_limits<float>::infinity();
    };

    float nextX = nextBoundary(origin.x, x, stepX);
    float nextY = nextBoundary(origin.y, y, stepY);
    float nextZ = nextBoundary(origin.z, z, stepZ);

    float tMaxX = (stepX == 0) ? std::numeric_limits<float>::infinity() : (nextX - origin.x) * invDx;
    float tMaxY = (stepY == 0) ? std::numeric_limits<float>::infinity() : (nextY - origin.y) * invDy;
    float tMaxZ = (stepZ == 0) ? std::numeric_limits<float>::infinity() : (nextZ - origin.z) * invDz;

    float tDeltaX = (stepX == 0) ? std::numeric_limits<float>::infinity() : (float)stepX * invDx; // abs(1/dx)
    float tDeltaY = (stepY == 0) ? std::numeric_limits<float>::infinity() : (float)stepY * invDy;
    float tDeltaZ = (stepZ == 0) ? std::numeric_limits<float>::infinity() : (float)stepZ * invDz;

    tDeltaX = std::fabs(tDeltaX);
    tDeltaY = std::fabs(tDeltaY);
    tDeltaZ = std::fabs(tDeltaZ);

    Int3 prevCell{ x,y,z };

    // 시작 셀 자체가 solid면 (카메라가 블록 안에 있을 때) 처리할지 여부는 취향
    // 여기선 "시작 셀도 검사" 하되, normal을 0으로 둠
    {
        BLOCK_ID id = world.GetBlock(x, y, z);
        if (world.IsSolid(id))
        {
            hitResult.bHit = true;
            hitResult.block = { x,y,z };
            hitResult.prev = { x,y,z };
            hitResult.normal = { 0,0,0 };
            hitResult.t = 0.f;
            return true;
        }
    }

    float t = 0.f;

    // DDA loop
    while (t <= maxDist)
    {
        prevCell = { x,y,z };

        // 가장 가까운 축 경계로 이동
        if (tMaxX < tMaxY && tMaxX < tMaxZ)
        {
            x += stepX;
            t = tMaxX;
            tMaxX += tDeltaX;

            // 방금 x경계를 넘어왔으니, 맞은 면 normal은 -stepX 방향
            hitResult.normal = { -stepX, 0, 0 };
        }
        else if (tMaxY < tMaxZ)
        {
            y += stepY;
            t = tMaxY;
            tMaxY += tDeltaY;
            hitResult.normal = { 0, -stepY, 0 };
        }
        else
        {
            z += stepZ;
            t = tMaxZ;
            tMaxZ += tDeltaZ;
            hitResult.normal = { 0, 0, -stepZ };
        }

        if (t > maxDist) break;

        BLOCK_ID id = world.GetBlock(x, y, z);
        if (world.IsSolid(id))
        {
            hitResult.bHit = true;
            hitResult.block = { x,y,z };
            hitResult.prev = prevCell;
            hitResult.t = t;
            return true;
        }
    }

    hitResult.bHit = false;
    return false;
}