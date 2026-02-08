#pragma once
#include "CScene.h"

class CTestScene : public CScene
{
public:
	CTestScene();
	virtual ~CTestScene();

	void Awake() override;
	void FixedUpdate(float fDelta) override;
	void Update(float fDelta) override;
	void LateUpdate(float fDelta) override;
	void BuildRenderFrame() override;

private:
	CObject* m_pTriangle = nullptr;
};