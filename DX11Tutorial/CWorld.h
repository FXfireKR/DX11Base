#pragma once
#include "CChunkWorld.h"

/*
CWorld
 └─ CChunkWorld : public IBlockAccessor
	  └─ ChunkColumn
		   └─ ChunkSection
				└─ BlockCell[]

*/

class CScene;
class CPipeline;
class CMaterial;

class CWorld
{
public:
	CWorld();
	~CWorld() = default;

	void Initialize(CScene& scene, CPipeline* pChunkPipeline, CMaterial* pChunkMaterial);
	void Update(float fDelta);
	void LateUpdate();

	bool RaycastBlock(IN const XMFLOAT3& origin, const XMFLOAT3& dirNorm, float maxDist, OUT BlockHitResult& outHitResult) const;
	bool TryPlaceBlock(int wx, int wy, int wz, const BlockCell& cell);
	bool TryBreakBlock(int wx, int wy, int wz);

	BlockCell GetBlockCell(int wx, int wy, int wz) const;
	bool IsSolidBlockAt(int wx, int wy, int wz) const;
	bool CheckAABBBlocked(const XMFLOAT3& center, const XMFLOAT3& halfExtents) const;

public:
	inline CChunkWorld& GetChunkWorld() { return *(m_pChunkWorld.get()); }
	inline const CChunkWorld& GetChunkWorld() const { return *(m_pChunkWorld.get()); }

private:
	unique_ptr<CChunkWorld> m_pChunkWorld;
};