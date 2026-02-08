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
	void Render() override;

	inline void SetMesh(CMesh* pMesh_) { m_pMesh = pMesh_; }
	void SetPipeline(CPipeline* pPipeline_) { m_pPipeline = pPipeline_; }

private:
	CMesh* m_pMesh = nullptr;
	CPipeline* m_pPipeline = nullptr;
};