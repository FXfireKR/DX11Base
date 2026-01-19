#include "pch.h"
#include "CScene.h"

void CScene::OnCreate()
{
	// 카메라가 존재하지 않으므로, 오브젝트를 만들어서 넣어준다.
	if (nullptr == m_pCurrentCamera) {
		ObjectID cameraID = m_objectManager.Add("MainCamera", this);

		CObject* pObject = m_objectManager.Get(cameraID);
		pObject->AddComponent<CTransform>();
		pObject->AddComponent<CCamera>();

		m_pCurrentCamera = m_objectManager.Get(cameraID)->GetComponent<CCamera>();
	}
}

void CScene::AddObject(const string& strName_)
{
	m_objectManager.Add(strName_, this);
}

void CScene::DestroyObject(const string& strName_)
{
	m_objectManager.Destroy(strName_);
}

void CScene::DestroyObject(ObjectID uObjectID_)
{
	m_objectManager.Destroy(uObjectID_);
}

const CObject* CScene::FindObject(const string& strName_) const
{
	return m_objectManager.Get(strName_);
}

const CObject* CScene::FindObject(ObjectID uObjectID_) const
{
	return m_objectManager.Get(uObjectID_);
}
