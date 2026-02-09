#pragma once
#include "CComponentBase.h"
#include "CMesh.h"
#include "CPipeline.h"

class CMeshRenderer : public CComponentBase<CMeshRenderer, COMPONENT_TYPE::MESHRENDER>
{
public:
	CMeshRenderer();
	~CMeshRenderer();

	void Init() override;

public:
	inline void SetMesh(CMesh* pMesh_) { m_pMesh = pMesh_; }
	inline void SetPipeline(CPipeline* pPipeline_) { m_pPipeline = pPipeline_; }
	inline CMesh* GetMesh() { return m_pMesh; }
	inline CPipeline* GetPipeline() { return m_pPipeline; }

private:
	CMesh* m_pMesh = nullptr;
	CPipeline* m_pPipeline = nullptr;
};