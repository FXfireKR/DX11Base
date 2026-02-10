#include "pch.h"
#include "CTestScene.h"
#include "CRenderWorld.h"

CTestScene::CTestScene()
{
}

CTestScene::~CTestScene()
{
}

void CTestScene::Awake()
{
	_CreateTriangle();
}

void CTestScene::Start()
{
}

void CTestScene::FixedUpdate(float fDelta)
{
	UNREFERENCED_PARAMETER(fDelta);
}

void CTestScene::Update(float fDelta)
{
	UNREFERENCED_PARAMETER(fDelta);

#ifdef IMGUI_ACTIVATE
	ImGui::Text("This is TestScene!");
#endif // IMGUI_ACTIVATE

	m_pTriangle->Update(fDelta);
}

void CTestScene::LateUpdate(float fDelta)
{
	UNREFERENCED_PARAMETER(fDelta);

	m_pTriangle->LateUpdate();

	GetCurrentCamera()->LateUpdate();
}

void CTestScene::BuildRenderFrame()
{
	CRenderWorld& rw = GetRenderWorld();

	GetCurrentCamera()->UpdateCameraMatrix();

	// matrix setting
	rw.SetViewMatrix(GetCurrentCamera()->GetViewMatrix());
	rw.SetProjectionMatrix(GetCurrentCamera()->GetProjMatrix());

	auto render = m_pTriangle->GetComponent<CMeshRenderer>();
	auto transform = m_pTriangle->GetComponent<CTransform>();
	
	RenderItem item;
	item.pMesh = render->GetMesh();
	item.pPipeline = render->GetPipeline();
	XMStoreFloat4x4(&item.world, XMMatrixTranspose(transform->GetWorldMatrix()));

	rw.Submit(item);
}

void CTestScene::_CreateTriangle()
{
	CRenderWorld& rw = GetRenderWorld();

	// shader
	auto& shaderManager = rw.GetShaderManager();
	auto shaderID = fnv1a_64("ColorForward");
	auto shader = shaderManager.CreateShader(shaderID, 0);

	shaderManager.Compile();
	
	// input layout
	auto& ilManager = rw.GetIALayoutManager();
	auto layoutID = ilManager.Create(VERTEX_POSITION_COLOR::GetLayout(), { shaderID, 0 }, shader->GetVertexBlob());

	// pipeline
	auto& pipelineManager = rw.GetPipelineManager();
	auto pipeID = pipelineManager.Create(fnv1a_64("TrianglePipeline"));
	pipelineManager.Get(pipeID)->SetShader(shaderManager.Get(shaderID, 0));
	pipelineManager.Get(pipeID)->SetInputLayout(ilManager.Get(layoutID));
	pipelineManager.Get(pipeID)->CreateRaster(rw.GetDevice());


	// mesh
	auto& meshManager = rw.GetMeshManager();
	auto meshID = meshManager.CreateTriangle(fnv1a_64("TriangleMesh"));

	// Object Create
	m_pTriangle = AddAndGetObject("TestObject #1");

	CTransform* const pTransform = m_pTriangle->AddComponent<CTransform>();

	CMeshRenderer* const pMeshRender = m_pTriangle->AddComponent<CMeshRenderer>();
	pMeshRender->SetMesh(meshManager.Get(meshID));
	pMeshRender->SetPipeline(pipelineManager.Get(pipeID));
}