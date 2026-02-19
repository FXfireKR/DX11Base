#include "pch.h"
#include "CScene.h"
#include "CGameWorld.h"

void CScene::OnCreate(CGameWorld& gameWorld)
{
	// renderWorld 가져오기
	m_pRenderWorld = gameWorld.GetRenderWorld();

	// 카메라가 존재하지 않으므로, 오브젝트를 만들어서 넣어준다.
	if (nullptr == m_pCurrentCamera) {
		ObjectID cameraID = m_objectManager.Add("MainCamera", this);

		CObject* pObject = m_objectManager.Get(cameraID);
		pObject->AddComponent<CTransform>();
		pObject->AddComponent<CCamera>();

		m_pCurrentCamera = m_objectManager.Get(cameraID)->GetComponent<CCamera>();
	}
}

void CScene::FixedUpdate(float fDelta)
{
	m_objectManager.ForEachAliveEnabled([&](CObject& obj) {
		obj.FixedUpdate(fDelta);
	});
}

void CScene::Update(float fDelta)
{
	m_objectManager.ForEachAliveEnabled([&](CObject& obj) {
		obj.Update(fDelta);
	});
}

void CScene::LateUpdate(float fDelta)
{
	m_objectManager.ForEachAliveEnabled([&](CObject& obj) {
		obj.LateUpdate(fDelta);
	});
}

void CScene::BuildRenderFrame()
{

}


ObjectID CScene::AddObject(const string& strName_)
{
	return m_objectManager.Add(strName_, this);
}

CObject* CScene::AddAndGetObject(const string& strName_)
{
	return FindObject(m_objectManager.Add(strName_, this));
}

void CScene::DestroyObject(const string& strName_)
{
	m_objectManager.Destroy(strName_);
}

void CScene::DestroyObject(ObjectID uObjectID_)
{
	m_objectManager.Destroy(uObjectID_);
}

CObject* CScene::FindObject(const string& strName_)
{
	return m_objectManager.Get(strName_);
}

CObject* CScene::FindObject(ObjectID uObjectID_)
{
	return m_objectManager.Get(uObjectID_);
}

const CObject* CScene::FindObject(const string& strName_) const
{
	return m_objectManager.Get(strName_);
}

const CObject* CScene::FindObject(ObjectID uObjectID_) const
{
	return m_objectManager.Get(uObjectID_);
}
