#pragma once
#include "VertexTypes.h"
#include "RenderPassTypes.h"

class CRenderWorld;
class CCamera;
class CMesh;
class CPipeline;
class CMaterial;

struct BlockCell;

struct BlockBreakParticle
{
	bool bAlive = false;
	XMFLOAT3 pos{};
	XMFLOAT3 vel{};

	XMFLOAT2 uvMin{};
	XMFLOAT2 uvMax{};

	float fSize = 0.1f;
	float fLife = 0.f;
	float fMaxLife = 0.f;
};

class CBlockBreakParticleSystem
{
public:
	void Initialize(CRenderWorld& rw);
	void Update(float fDelta);

	void SpawnBreakBurst(const XMINT3& blockPos, const BlockCell& cell, const XMINT3& hitNormal);
	void SubmitRender(CRenderWorld& rw, const CCamera& camera);

private:
	void _CreateRenderResource(CRenderWorld& rw);
	void _BuildDynamicMesh(CRenderWorld& rw, const CCamera& camera);
	void _EmitOne(const XMFLOAT3& pos, const XMFLOAT3& vel, float size, float life
		, const XMFLOAT2& uvMin, const XMFLOAT2& uvMax);
	float _RandomRange(float minV, float maxV) const;

private:
	vector<BlockBreakParticle> m_vecParticles;
	vector<VERTEX_POSITION_UV> m_vecVertices;
	vector<uint32_t> m_vecIndices;

	CMesh* m_pMesh = nullptr;
	CPipeline* m_pPipeline = nullptr;
	CMaterial* m_pMaterial = nullptr;

	uint64_t m_uMeshKey = 0;
	uint32_t m_uMaxParticles = 256;
};