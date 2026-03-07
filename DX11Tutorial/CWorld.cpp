#include "pch.h"
#include "CWorld.h"
#include "CChunkMesherSystem.h"

CWorld::CWorld()
    : m_pChunkWorld(make_unique<CChunkWorld>())
{
}

void CWorld::Initialize(CScene& scene, CPipeline* pChunkPipeline, CMaterial* pChunkMaterial)
{
    m_pChunkWorld->Initialize(scene, pChunkPipeline, pChunkMaterial);

	// Testing Code
	{
		BlockPropHashMap props;
		//props[fnv1a_64("snowy")] = fnv1a_64("false");

		BLOCK_ID blockID = fnv1a_64("minecraft:stone");

		STATE_INDEX sidx;
		bool ok = CBlockStateDB::Get().EncodeStateIndex(blockID, props, sidx);
		assert(ok);

		// 테스트용 평면 생성
		for (int z = -64; z < 64; ++z)
		{
			for (int x = -64; x < 64; ++x)
			{
				m_pChunkWorld->SetBlock(x, 0, z, { blockID, sidx }); // y=0 바닥
			}
		}

		// 테스트용 기둥
		for (int y = 1; y < 5; ++y)
		{
			m_pChunkWorld->SetBlock(0, y, 0, { blockID, sidx });
		}
	}
}

void CWorld::Update(float fDelta)
{
}

void CWorld::LateUpdate()
{
}

bool CWorld::RaycastBlock(IN const XMFLOAT3& origin, const XMFLOAT3& dirNorm, float maxDist, OUT BlockHitResult& outHitResult) const
{
    return RaycastVoxelDDA(*m_pChunkWorld, origin, dirNorm, maxDist, outHitResult);
}

bool CWorld::TryPlaceBlock(int wx, int wy, int wz, const BlockCell& cell)
{
	const BlockCell cur = m_pChunkWorld->GetBlockCell(wx, wy, wz);
	if (!cur.IsAir()) return false;

	m_pChunkWorld->SetBlock(wx, wy, wz, cell);
	return true;
}

bool CWorld::TryBreakBlock(int wx, int wy, int wz)
{
	const BlockCell cur = m_pChunkWorld->GetBlockCell(wx, wy, wz);
	if (cur.IsAir()) return false;

	m_pChunkWorld->SetBlock(wx, wy, wz, BlockCell{ 0, 0 });
	return true;
}

BlockCell CWorld::GetBlockCell(int wx, int wy, int wz) const
{
	return m_pChunkWorld->GetBlockCell(wx, wy, wz);
}

bool CWorld::IsSolidBlockAt(int wx, int wy, int wz) const
{
	const BlockCell cell = GetBlockCell(wx, wy, wz);
	return m_pChunkWorld->IsSolid(cell);
}

bool CWorld::CheckAABBBlocked(const XMFLOAT3& center, const XMFLOAT3& halfExtents) const
{
	const XMFLOAT3 fMin =
	{
		center.x - halfExtents.x,
		center.y - halfExtents.y,
		center.z - halfExtents.z
	};

	const XMFLOAT3 fMax =
	{
		center.x + halfExtents.x,
		center.y + halfExtents.y,
		center.z + halfExtents.z
	};

	const XMINT3 iMin =
	{
		static_cast<int>(std::floor(fMin.x)),
		static_cast<int>(std::floor(fMin.y)),
		static_cast<int>(std::floor(fMin.z))
	};

	const XMINT3 iMax =
	{
		static_cast<int>(std::floor(fMax.x)),
		static_cast<int>(std::floor(fMax.y)),
		static_cast<int>(std::floor(fMax.z))
	};

	for (int y = iMin.y; y <= iMax.y; ++y)
	{
		for (int z = iMin.z; z <= iMax.z; ++z)
		{
			for (int x = iMin.x; x <= iMax.x; ++x)
			{
				if (IsSolidBlockAt(x, y, z)) return true;
			}
		}
	}
	return false;
}