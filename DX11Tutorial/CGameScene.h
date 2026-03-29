#pragma once
#include "CScene.h"

class CGameScene : public CScene
{
public:
	CGameScene();
	virtual ~CGameScene();

	void Awake() override;
	void Start() override;

	void FixedUpdate(float fDelta) override;
	void Update(float fDelta) override;
	void LateUpdate(float fDelta) override;

	void BuildRenderFrame() override;

};

