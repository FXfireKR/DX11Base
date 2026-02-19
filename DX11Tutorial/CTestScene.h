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
	void _CreateTriangle();

private:
	CObject* m_pTriangle = nullptr;
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
};