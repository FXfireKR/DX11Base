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
	CScene::FixedUpdate(fDelta);
}

void CTestScene::Update(float fDelta)
{
	CScene::Update(fDelta);

#ifdef IMGUI_ACTIVATE
	ImGui::Text("This is TestScene!");
	ImGui::Text("Delta X : %d", CInputManager::Get().Mouse().GetDelta().x);
	ImGui::Text("Delta Y : %d", CInputManager::Get().Mouse().GetDelta().y);

	ImGui::Text("Rotate X : %.3f", x);
	ImGui::Text("Rotate Y : %.3f", y);
	ImGui::Text("Rotate Z : %.3f", z);
#endif // IMGUI_ACTIVATE

	//x += 45.f * fDelta;
	/*if (x > 89.9f) {
		x -= 179.9;
	}*/

	y += 45.f * fDelta;
	/*if (y > 360.0f) {
		y -= 360.0f;
	}*/

	CTransform* pTrans = m_pTriangle->GetComponent<CTransform>();
	pTrans->SetLocalRotateEulerDeg({ x,y,z });
}

void CTestScene::LateUpdate(float fDelta)
{
	CScene::LateUpdate(fDelta);
}

void CTestScene::BuildRenderFrame()
{
	CRenderWorld& rw = GetRenderWorld();

	//GetCurrentCamera()->UpdateCameraMatrix();

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
	auto shaderID = fnv1a_64("NormalImageForward");
	auto shader = shaderManager.CreateShader(shaderID, 0);

	shaderManager.Compile();
	
	// input layout
	auto& ilManager = rw.GetIALayoutManager();
	auto layoutID = ilManager.Create(VERTEX_POSITION_UV_NORMAL::GetLayout(), { shaderID, 0 }, shader->GetVertexBlob());

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
	auto meshID = meshManager.CreateCube(fnv1a_64("TriangleMesh"));

	// texture
	auto& textureManager = rw.GetTextureManager();
	auto textureID = textureManager.LoadTexture2D(fnv1a_64("Gaki"), "../Resource/stone.png", TEXTURE_USAGE::StaticColorMip);

	// sampler
	auto& samplerManager = rw.GetSamplerManager();
	auto samplerID = samplerManager.Create(SAMPLER_TYPE::POINT_WRAP);

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