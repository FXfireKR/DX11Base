#include "pch.h"
#include "CTestScene.h"

CTestScene::CTestScene()
{
}

CTestScene::~CTestScene()
{
}

void CTestScene::Awake()
{
	m_pTriangle = AddAndGetObject("test_mesh");
	
	CMeshRenderer* const pMeshRender = m_pTriangle->AddComponent<CMeshRenderer>();
}

void CTestScene::FixedUpdate(float fDelta)
{
}

void CTestScene::Update(float fDelta)
{
#ifdef IMGUI_ACTIVATE
	ImGui::Text("This is TestScene!");
#endif // IMGUI_ACTIVATE
}

void CTestScene::LateUpdate(float fDelta)
{
}

void CTestScene::BuildRenderFrame()
{
	
}