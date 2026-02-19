#pragma once
#include "CMesh.h"

class CMeshManager
{
public:
	CMeshManager() = default;
	~CMeshManager() = default;

	void Initialize(ID3D11Device& refDevice_);
	uint64_t Create(uint64_t id);
	CMesh* Get(uint64_t id);
	const CMesh* Get(uint64_t id) const;

public:
	uint64_t CreateTriangle(uint64_t id);
	uint64_t CreateQuad(uint64_t id);
	uint64_t CreateCube(uint64_t id);

private:
	std::unordered_map<uint64_t, unique_ptr<CMesh>> m_mapMesh;

	ID3D11Device* m_pDevice = nullptr;
};