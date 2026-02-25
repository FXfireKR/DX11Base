#pragma once
#include "CObjectManager.h"

class CGameWorld;
class CRenderWorld;

enum class SCENE_TYPE : unsigned char
{
	BOOT_SCENE = 0,
	INIT_SCENE,
	TEST_SCENE,


	END_SCENE,
};
constexpr uint32_t SCENE_MAX_SIZE = static_cast<uint32_t>(SCENE_TYPE::END_SCENE);

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
	virtual void FixedUpdate(float fDelta);
	virtual void Update(float fDelta);
	virtual void LateUpdate(float fDelta);

	// Building Frame
	virtual void BuildRenderFrame();

public:
	// Object 중계
	ObjectID AddObject(const string& strName_);
	CObject* AddAndGetObject(const string& strName_);

	void DestroyObject(const string& strName_);
	void DestroyObject(ObjectID uObjectID_);

	CObject* FindObject(const string& strName_);
	CObject* FindObject(ObjectID uObjectID_);

	const CObject* FindObject(const string& strName_) const;
	const CObject* FindObject(ObjectID uObjectID_) const;

public:
	CObjectManager& GetObjectManager() { return m_objectManager; }
	CCamera* GetCurrentCamera() { return m_pCurrentCamera; }
	const CCamera* GetCurrentCamera() const { return m_pCurrentCamera; }

	inline CRenderWorld& GetRenderWorld() { return *m_pRenderWorld; }
	inline const bool& IsRequestForChange() { return m_bChangeSceneReq; }
	inline const SCENE_TYPE& GetNextScene() { return m_eNextSceneReq; }

protected:
	CObjectManager m_objectManager;
	CCamera* m_pCurrentCamera = nullptr;
	CRenderWorld* m_pRenderWorld = nullptr; // not-own

	bool m_bChangeSceneReq = false;
	SCENE_TYPE m_eNextSceneReq = SCENE_TYPE::END_SCENE;
};