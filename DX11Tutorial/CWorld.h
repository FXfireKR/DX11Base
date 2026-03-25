#pragma once
#include "CChunkWorld.h"
#include "CWorldTime.h"
#include "BlockRaycastTypes.h"

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

	void Initialize(CScene& scene, CPipeline* pOpaquePipeline, CMaterial* pOpaqueMaterial
		, CPipeline* pCutoutPipeline, CMaterial* pCutoutMaterial
		, CPipeline* pTranslucentPipeline, CMaterial* pTranslucentMaterial);
	void Update(float fDelta, XMFLOAT3 pos);
	void LateUpdate(CScene& scene);

	bool RaycastBlock(IN const XMFLOAT3& origin, const XMFLOAT3& dirNorm, float maxDist, OUT BlockHitResult& outHitResult) const;
	bool TryPlaceBlock(int wx, int wy, int wz, const BlockCell& cell);
	bool TryBreakBlock(int wx, int wy, int wz);

	BlockCell GetBlockCell(int wx, int wy, int wz) const;
	bool IsSolidBlockAt(int wx, int wy, int wz) const;
	bool CheckAABBBlocked(const XMFLOAT3& center, const XMFLOAT3& halfExtents) const;
	bool FindSpawnFootY(int wx, int wz, const XMFLOAT3& halfExtents, float& outFootY) const;

public:
	inline CChunkWorld& GetChunkWorld() { return *(m_pChunkWorld.get()); }
	inline const CChunkWorld& GetChunkWorld() const { return *(m_pChunkWorld.get()); }

	inline CWorldTime& GetWorldTime() { return m_worldTime; }
	inline const CWorldTime& GetWorldTime() const { return m_worldTime; }

private:
	unique_ptr<CChunkWorld> m_pChunkWorld;
	CWorldTime m_worldTime;
};