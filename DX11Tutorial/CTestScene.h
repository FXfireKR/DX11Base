#pragma once
#include "CScene.h"

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
	void _CreateTriangle();

private:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;

	CObject* m_pChunkObject = nullptr;
	CPipeline* m_pChunkPipeline = nullptr;
	CMaterial* m_pChunkMaterial = nullptr;
};