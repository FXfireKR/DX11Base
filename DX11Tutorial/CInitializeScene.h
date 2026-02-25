#pragma once
#include "CScene.h"

class CInitializeScene : public CScene
{
public:
	CInitializeScene() = default;
	virtual ~CInitializeScene() = default;

	void Awake() override;
	void FixedUpdate(float fDelta) override;
	void Update(float fDelta) override;
	void LateUpdate(float fDelta) override;
};