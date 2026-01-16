#pragma once
#include "CScene.h"

class CTestScene : public CScene
{
public:
	CTestScene();
	virtual ~CTestScene();

	void Awake() override;
	void FixedUpdate() override;
	void Update() override;
	void LateUpdate() override;


private:

};