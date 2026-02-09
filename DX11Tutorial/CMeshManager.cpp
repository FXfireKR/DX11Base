#include "pch.h"
#include "CMeshManager.h"

void CMeshManager::Initialize(ID3D11Device& refDevice_)
{
	m_pDevice = &refDevice_;
}

uint64_t CMeshManager::Create(uint64_t id)
{
	auto iter = m_mapMesh.find(id);
	if (iter == m_mapMesh.end()) {
		m_mapMesh.insert(make_pair(id, make_unique<CMesh>()));
	}
	return id;
}

CMesh* CMeshManager::Get(uint64_t id)
{
	auto iter = m_mapMesh.find(id);
	if (iter != m_mapMesh.end()) {
		return iter->second.get();
	}
	return nullptr;
}

const CMesh* CMeshManager::Get(uint64_t id) const
{
	auto iter = m_mapMesh.find(id);
	if (iter != m_mapMesh.end()) {
		return iter->second.get();
	}
	return nullptr;
}

uint64_t CMeshManager::CreateTriangle(uint64_t id)
{
	auto meshID = this->Create(id);
	this->Get(meshID)->CreateTriangle(m_pDevice);
	return id;
}