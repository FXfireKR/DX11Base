#pragma once
#include "CObjectManager.h"

class CGameWorld;
class CRenderWorld;

class CScene
{
public:
	CScene() {}
	virtual ~CScene() {}

	// Scene 관련 동작
	virtual void OnCreate(CGameWorld& gameWorld);

public: // Object 라이프 사이클
	// Initialization
	virtual void Awake() {} // next Load 단계
	virtual void Start() {} // current 활성화 단계

	// Physics
	virtual void FixedUpdate(float fDelta) { UNREFERENCED_PARAMETER(fDelta); }
	virtual void Update(float fDelta) { UNREFERENCED_PARAMETER(fDelta); }
	virtual void LateUpdate(float fDelta) { UNREFERENCED_PARAMETER(fDelta); }

	// Building Frame
	virtual void BuildRenderFrame() {}

public: // Object 중계
	ObjectID AddObject(const string& strName_);
	CObject* AddAndGetObject(const string& strName_);

	void DestroyObject(const string& strName_);
	void DestroyObject(ObjectID uObjectID_);

	CObject* FindObject(const string& strName_);
	CObject* FindObject(ObjectID uObjectID_);

	const CObject* FindObject(const string& strName_) const;
	const CObject* FindObject(ObjectID uObjectID_) const;

public: // Current camera 중계
	const CCamera* GetCurrentCamera() const { return m_pCurrentCamera; }

protected:
	CRenderWorld& GetRenderWorld() { return *m_pRenderWorld; }

private:
	CObjectManager m_objectManager;
	CCamera* m_pCurrentCamera = nullptr;
	CRenderWorld* m_pRenderWorld = nullptr; // not-own
};