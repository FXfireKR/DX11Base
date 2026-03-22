#pragma once
#include "CScene.h"
#include "CWorld.h"

enum class ESectionDebugMode : uint8_t
{
	OFF = 0,
	EXIST_ONLY,
	ALL,

	COUNT,
};

class CTestScene : public CScene
{
public:
	CTestScene();
	virtual ~CTestScene();

	void Awake() override;
	void Start() override;

	void FixedUpdate(float fDelta) override;
	void Update(float fDelta) override;
	void LateUpdate(float fDelta) override;

	void BuildRenderFrame() override;

private:
	void _CreateChunkObject();
	void _CreateBaked();
	void _MakeCenterRay(IN const CCamera& cam, OUT XMFLOAT3& orig, OUT XMFLOAT3& dir);
	void _CreateHighlight();
	void _CreateWorldRender();
	void _CreateTextureAtlas();

	void _CreateSkyBillboardResources();
	void _SubmitSunMoonBillboards(CRenderWorld& rw);

	XMFLOAT3 _LerpColor(const XMFLOAT3& a, const XMFLOAT3& b, float t);
	void _ApplySkyClearColor();

	XMMATRIX _BuildSkyLockedQuadWorld(const XMFLOAT3& center, const XMFLOAT3& dirFromCam
		, float width, float height);
	XMMATRIX _BuildScreenAlignedBillboardWorld(const XMFLOAT3& center, const XMFLOAT3& camRight
		, const XMFLOAT3& camUp, float width, float height);
	void _CalcSunMoonDirection(XMFLOAT3& outSunDir, XMFLOAT3& outMoonDir) const;

	void _SubmitChunkBoundsDebug(CRenderWorld& rw) const;
	void _SubmitSectionBoundsDebug(CRenderWorld& rw) const;

private:
	CPipeline* m_pChunkPipeline = nullptr;
	CMaterial* m_pChunkMaterial = nullptr;
	CWorld m_VoxelWorld;
	WorldTimeParams timeParams{};

	CObject* m_pPlayer = nullptr;
	CObject* m_pHighlightObject = nullptr;

	bool m_bShowChunkBounds = false;
	CMesh* m_pChunkBoundsDebugMesh = nullptr;
	CPipeline* m_pChunkBoundsDebugPipeline = nullptr;
	CMaterial* m_pChunkBoundsDebugMaterial = nullptr;

	ESectionDebugMode m_eSectionDebugMode = ESectionDebugMode::OFF;

private:
	CMesh* m_pSkyBillboardMesh = nullptr;
	CPipeline* m_pSkyBillboardPipeline = nullptr;
	CMaterial* m_pSunBillboardMaterial = nullptr;
	CMaterial* m_pMoonBillboardMaterial = nullptr;

	float m_fSkyBillboardRadius = 400.f;
	float m_fSunBillboardSize = 250.f;
	float m_fMoonBillboardSize = 225.f;
	bool m_bShowSunMoon = true;
	bool m_bSpawnStreamingReady = false;
};