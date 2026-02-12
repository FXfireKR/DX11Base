#pragma once
#include "CComponentBase.h"
#include "CMesh.h"
#include "CPipeline.h"
#include "CMaterial.h"

class CMeshRenderer : public CComponentBase<CMeshRenderer, COMPONENT_TYPE::MESHRENDER>
{
public:
	CMeshRenderer();
	~CMeshRenderer();

	void Init() override;

public:
	inline void SetMesh(CMesh* pMesh_) { m_pMesh = pMesh_; }
	inline void SetPipeline(CPipeline* pPipeline_) { m_pPipeline = pPipeline_; }
	inline void SetMaterial(CMaterial* pMaterial_) { m_pMaterial = pMaterial_; }

public:
	inline const CMesh* const GetMesh() { return m_pMesh; }
	inline const CPipeline* const GetPipeline() { return m_pPipeline; }
	inline const CMaterial* const GetMaterial() { return m_pMaterial; }

private:
	CMesh* m_pMesh = nullptr;
	CPipeline* m_pPipeline = nullptr;
	CMaterial* m_pMaterial = nullptr;
};