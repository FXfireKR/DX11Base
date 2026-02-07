#pragma once
#include "CObjectManager.h"

class CScene
{
public:
	CScene() {}
	virtual ~CScene() {}

	// Scene 관련 동작
	virtual void OnCreate();

public: // Object 라이프 사이클
	virtual void Awake() {}
	virtual void FixedUpdate(float fDelta) {}
	virtual void Update(float fDelta) {}
	virtual void LateUpdate(float fDelta) {}

public: // Object 중계
	void AddObject(const string& strName_);
	void DestroyObject(const string& strName_);
	void DestroyObject(ObjectID uObjectID_);
	const CObject* FindObject(const string& strName_) const;
	const CObject* FindObject(ObjectID uObjectID_) const;

public: // Current camera 중계
	const CCamera* GetCurrentCamera() const { return m_pCurrentCamera; }

private:
	CObjectManager m_objectManager;
	CCamera* m_pCurrentCamera = nullptr;
};