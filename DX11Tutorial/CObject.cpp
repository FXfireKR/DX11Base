#include "pch.h"
#include "CObject.h"

CObject::CObject()
{
}

CObject::~CObject()
{
}

void CObject::Init()
{
	for (const auto& iter : m_arrComponents) {
		iter->Init();
	}
}

void CObject::FixedUpdate()
{
	for (const auto& iter : m_arrComponents) {
		if (iter->GetAlive() && iter->GetEnable()) {
			iter->FixedUpdate();
		}
	}
}

void CObject::Update(float fDelta)
{
	for (const auto& iter : m_arrComponents) {
		if (iter->GetAlive() && iter->GetEnable()) {
			iter->Update(fDelta);
		}
	}
}

void CObject::LateUpdate()
{
	for (const auto& iter : m_arrComponents) {
		if (iter->GetAlive() && iter->GetEnable()) {
			iter->LateUpdate();
		}
	}
}

void CObject::Render()
{
	for (const auto& iter : m_arrComponents) {
		if (iter->GetAlive() && iter->GetEnable()) {
			iter->Render();
		}
	}
}

void CObject::Destroy()
{
	m_bAlive = false;
	m_bPeddingDestroy = true;
}

void CObject::AddChild(ObjectID uChildID_)
{
	if (std::find(m_vecChildren.begin(), m_vecChildren.end(), uChildID_) == m_vecChildren.end()) {
		m_vecChildren.push_back(uChildID_);
	}
}