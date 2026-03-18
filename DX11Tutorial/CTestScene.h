#pragma once
#include "CScene.h"
#include "CWorld.h"

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

	void _SubmitChunkBoundsDebug(CRenderWorld& rw) const;

private:
	CPipeline* m_pChunkPipeline = nullptr;
	CMaterial* m_pChunkMaterial = nullptr;
	CWorld m_VoxelWorld;

	CObject* m_pPlayer = nullptr;
	CObject* m_pHighlightObject = nullptr;

	bool m_bShowChunkBounds = false;
	CMesh* m_pChunkBoundsDebugMesh = nullptr;
	CPipeline* m_pChunkBoundsDebugPipeline = nullptr;
	CMaterial* m_pChunkBoundsDebugMaterial = nullptr;
};