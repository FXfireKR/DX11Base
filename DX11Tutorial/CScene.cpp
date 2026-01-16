#include "pch.h"
#include "CScene.h"

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
