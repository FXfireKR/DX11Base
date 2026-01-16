#pragma once
#include "CObjectManager.h"

class CScene
{
public:
	CScene() {}
	virtual ~CScene() {}

	virtual void Awake() {}
	virtual void FixedUpdate() {}
	virtual void Update() {}
	virtual void LateUpdate() {}

public: // Object 중계
	void AddObject(const string& strName_);
	void DestroyObject(const string& strName_);
	void DestroyObject(ObjectID uObjectID_);
	const CObject* FindObject(const string& strName_) const;
	const CObject* FindObject(ObjectID uObjectID_) const;

private:
	CObjectManager m_objectManager;
	CCamera* m_pCurrentCamera = nullptr;
};