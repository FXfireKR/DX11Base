#include "pch.h"
#include "CMeshRenderer.h"

CMeshRenderer::CMeshRenderer()
{
}

CMeshRenderer::~CMeshRenderer()
{
}

void CMeshRenderer::Init()
{
}

void CMeshRenderer::Initialize(CMesh* const pMesh_, CPipeline* const pPipeline_)
{
	m_pMesh = pMesh_;
	m_pPipeline = pPipeline_;
}

void CMeshRenderer::Render()
{
}
