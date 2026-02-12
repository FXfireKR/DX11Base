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

	m_pTriangle->LateUpdate(fDelta);

	GetCurrentCamera()->LateUpdate(fDelta);
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
	item.pMaterial = render->GetMaterial();

	XMStoreFloat4x4(&item.world, XMMatrixTranspose(transform->GetWorldMatrix()));

	rw.Submit(item);
}

void CTestScene::_CreateTriangle()
{
	CRenderWorld& rw = GetRenderWorld();

	// shader
	auto& shaderManager = rw.GetShaderManager();
	auto shaderID = fnv1a_64("ImageForward");
	auto shader = shaderManager.CreateShader(shaderID, 0);

	shaderManager.Compile();
	
	// input layout
	auto& ilManager = rw.GetIALayoutManager();
	auto layoutID = ilManager.Create(VERTEX_POSITION_UV::GetLayout(), { shaderID, 0 }, shader->GetVertexBlob());

	// pipeline
	auto& pipelineManager = rw.GetPipelineManager();
	auto pipeID = pipelineManager.Create(fnv1a_64("TrianglePipeline"));

	auto pipeline = pipelineManager.Get(pipeID);

	pipeline->SetShader(shaderManager.Get(shaderID, 0));
	pipeline->SetInputLayout(ilManager.Get(layoutID));

	// dummy
	pipeline->CreateRaster(rw.GetDevice());

	// mesh
	auto& meshManager = rw.GetMeshManager();
	auto meshID = meshManager.CreateQuad(fnv1a_64("TriangleMesh"));

	// texture
	auto& textureManager = rw.GetTextureManager();
	auto textureID = textureManager.LoadTexture2D(fnv1a_64("Gaki"), "../Resource/image.jpg", TEXTURE_USAGE::StaticColor);

	// sampler
	auto& samplerManager = rw.GetSamplerManager();
	auto samplerID = samplerManager.Create(SAMPLER_TYPE::LINEAR_WARP);

	// material
	auto& materialManager = rw.GetMaterialManager();
	auto materialID = materialManager.Create(fnv1a_64("GakiDrawer"));
	materialManager.Get(materialID)->SetTexture(0, textureManager.GetTexture(textureID)->GetShaderResourceView());
	materialManager.Get(materialID)->SetSampler(0, samplerManager.Get(samplerID)->Get());
	
	
	// Object Create
	m_pTriangle = AddAndGetObject("TestObject #1");

	CTransform* const pTransform = m_pTriangle->AddComponent<CTransform>();

	// Set Mesh Render
	CMeshRenderer* const pMeshRender = m_pTriangle->AddComponent<CMeshRenderer>();
	pMeshRender->SetMesh(meshManager.Get(meshID));
	pMeshRender->SetPipeline(pipelineManager.Get(pipeID));
	pMeshRender->SetMaterial(materialManager.Get(materialID));
}