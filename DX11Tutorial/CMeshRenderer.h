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
	void Initialize(CMesh* const pMesh_, CPipeline* const pPipeline_);

	void Render() override;

private:
	CMesh* m_pMesh = nullptr;
	CPipeline* m_pPipeline = nullptr;

};