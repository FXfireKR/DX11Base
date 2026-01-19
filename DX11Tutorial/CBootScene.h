#pragma once
#include "CScene.h"

class CBootScene : public CScene
{
public:
	CBootScene();
	virtual ~CBootScene();

	void Awake() override;
	void FixedUpdate() override;
	void Update() override;
	void LateUpdate() override;

private:
};

