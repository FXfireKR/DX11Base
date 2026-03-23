#pragma once
#include "CComponentBase.h"
#include "CMesh.h"
#include "CPipeline.h"
#include "CMaterial.h"
#include "RenderPassTypes.h"

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
	inline void SetRenderPass(ERenderPass ePass_) { m_eRenderPass = ePass_; }

	inline const CMesh* const GetMesh() { return m_pMesh; }
	inline const CPipeline* const GetPipeline() { return m_pPipeline; }
	inline const CMaterial* const GetMaterial() { return m_pMaterial; }
	inline const ERenderPass GetRenderPass() const { return m_eRenderPass; }

private:
	CMesh* m_pMesh = nullptr;
	CPipeline* m_pPipeline = nullptr;
	CMaterial* m_pMaterial = nullptr;
	ERenderPass m_eRenderPass = ERenderPass::OPAQUE_PASS;
};