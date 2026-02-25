#include "pch.h"
#include "CTestScene.h"
#include "CRenderWorld.h"
#include "CChunkMesherSystem.h"

CTestScene::CTestScene()
{
}

CTestScene::~CTestScene()
{
}

void CTestScene::Awake()
{
	CModelDB::Get().Initialize("../Resource/");
	CBlockStateDB::Get().Initialize("../Resource/");

	_CreateBaked();
	//_CreateTriangle();
	//_CreateChunkObject();
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


	auto& input = CInputManager::Get();
	auto& keyboard = input.Keyboard();

	static float speeeed = 45.f;

	CTransform* pTransform = m_pChunkObject->GetComponent<CTransform>();
	if (keyboard.GetKey('W')) {
		x += fDelta * speeeed;
	}

	if (keyboard.GetKey('S')) {
		x -= fDelta * speeeed;
	}

	if (keyboard.GetKey('A')) {
		y -= fDelta * speeeed;
	}

	if (keyboard.GetKey('D')) {
		y += fDelta * speeeed;
	}

	if (keyboard.GetKey('E')) {
		z -= fDelta * speeeed;
	}

	if (keyboard.GetKey('Q')) {
		z += fDelta * speeeed;
	}

	pTransform->SetLocalRotateEulerDeg({ x,y,z });	
}

void CTestScene::LateUpdate(float fDelta)
{
	CScene::LateUpdate(fDelta);
}

void CTestScene::BuildRenderFrame()
{
	CRenderWorld& rw = GetRenderWorld();

	// matrix setting
	rw.SetViewMatrix(GetCurrentCamera()->GetViewMatrix());
	rw.SetProjectionMatrix(GetCurrentCamera()->GetProjMatrix());

	auto render = m_pChunkObject->GetComponent<CMeshRenderer>();
	auto transform = m_pChunkObject->GetComponent<CTransform>();

	//CChunkMesherSystem::RebuildDirtyChunks(*this);
	
	RenderItem item;
	item.pMesh = render->GetMesh();
	item.pPipeline = render->GetPipeline();
	item.pMaterial = render->GetMaterial();

	XMStoreFloat4x4(&item.world, XMMatrixTranspose(transform->GetWorldMatrix()));

	rw.Submit(item);
}

void CTestScene::_CreateChunkObject()
{
	CRenderWorld& rw = GetRenderWorld();

	RuntimeAtlasDesc ad{};
	ad.width = 256;
	ad.height = 256;
	ad.tilePx = 16;
	ad.eFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	ad.bSRGB = true;
	ad.bMipmap = false;

	rw.GetRuntimeAtlas().Create(rw.GetDevice(), ad);

	rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("bedrock"), "../Resource/assets/minecraft/textures/block/bedrock.png");
	rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("bricks"), "../Resource/assets/minecraft/textures/block/bricks.png");
	rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("glass"), "../Resource/assets/minecraft/textures/block/glass.png");
	rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("gravel"), "../Resource/assets/minecraft/textures/block/gravel.png");
	rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("sand"), "../Resource/assets/minecraft/textures/block/sand.png");
	rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("stone"), "../Resource/assets/minecraft/textures/block/stone.png");

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
	auto pipeID = pipelineManager.Create(fnv1a_64("ChunkPipeline"));
	auto pipeline = pipelineManager.Get(pipeID);

	pipeline->SetShader(shaderManager.Get(shaderID, 0));
	pipeline->SetInputLayout(ilManager.Get(layoutID));
	pipeline->CreateRaster(rw.GetDevice());

	// texture
	//auto& textureManager = rw.GetTextureManager();
	//auto textureID = textureManager.LoadTexture2D(fnv1a_64("Stone"), "../Resource/stone.png", TEXTURE_USAGE::StaticColorMip);

	// sampler
	auto& samplerManager = rw.GetSamplerManager();
	auto samplerID = samplerManager.Create(SAMPLER_TYPE::POINT_WRAP);

	// material
	auto& materialManager = rw.GetMaterialManager();
	auto materialID = materialManager.Create(fnv1a_64("ChunkMaterial"));
	materialManager.Get(materialID)->SetTexture(0, rw.GetRuntimeAtlas().GetShaderResourceView());
	materialManager.Get(materialID)->SetSampler(0, samplerManager.Get(samplerID)->Get());

	m_pChunkPipeline = pipelineManager.Get(pipeID);
	m_pChunkMaterial = materialManager.Get(materialID);

	m_pChunkObject = AddAndGetObject("Chunk_0_0_0");
	auto* tr = m_pChunkObject->AddComponent<CTransform>();
	tr->SetLocalTrans({ 0.f, 0.f, 0.f });

	auto* mr = m_pChunkObject->AddComponent<CMeshRenderer>();
	mr->SetPipeline(m_pChunkPipeline);
	mr->SetMaterial(m_pChunkMaterial);

	auto* cnk = m_pChunkObject->AddComponent<CChunkComponent>();
	cnk->Init();
	cnk->SetMeshRenderer(mr);
	cnk->SetChunkCoord({ 0,0,0 });

	for (int z = 0; z < CHUNK_SIZE_Z; ++z)
		for (int y = 0; y < CHUNK_SIZE_Y; ++y)
			for (int x = 0; x < CHUNK_SIZE_X; ++x)
				cnk->SetBlock(x, y, z, rand() % 7);
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
	//m_pTriangle = AddAndGetObject("TestObject #1");

	//CTransform* const pTransform = m_pTriangle->AddComponent<CTransform>();

	//// Set Mesh Render
	//CMeshRenderer* const pMeshRender = m_pTriangle->AddComponent<CMeshRenderer>();
	//pMeshRender->SetMesh(meshManager.Get(meshID));
	//pMeshRender->SetPipeline(pipelineManager.Get(pipeID));
	//pMeshRender->SetMaterial(materialManager.Get(materialID));
}

void CTestScene::_CreateBaked()
{
	CRenderWorld& rw = GetRenderWorld();

	RuntimeAtlasDesc ad{};
	ad.width = 256;
	ad.height = 256;
	ad.tilePx = 16;
	ad.eFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	ad.bSRGB = true;
	ad.bMipmap = false;

	rw.GetRuntimeAtlas().Create(rw.GetDevice(), ad);

	rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("block/grass_block_snow"), "../Resource/assets/minecraft/textures/block/grass_block_snow.png");
	rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("block/grass_block_side"), "../Resource/assets/minecraft/textures/block/grass_block_side.png");
	rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("block/grass_block_side_overlay"), "../Resource/assets/minecraft/textures/block/grass_block_side_overlay.png");
	rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("block/grass_block_top"), "../Resource/assets/minecraft/textures/block/grass_block_top.png");
	rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("block/dirt"), "../Resource/assets/minecraft/textures/block/dirt.png");

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

	BlockPropHashMap props;
	props[fnv1a_64("snowy")] = fnv1a_64("false");

	BLOCK_ID blockID = fnv1a_64("minecraft:grass_block");

	STATE_INDEX sidx;
	bool ok = CBlockStateDB::Get().EncodeStateIndex(blockID, props, sidx);
	assert(ok);

	std::vector<AppliedModel> outModels;
	CBlockStateDB::Get().GetAppliedModels(blockID, sidx, outModels);
	auto it = outModels.front();
	auto meshID = meshManager.CreateMeshFromBakedModel(CModelDB::Get().GetModel(it.modelHash), rw.GetRuntimeAtlas());

	// sampler
	auto& samplerManager = rw.GetSamplerManager();
	auto samplerID = samplerManager.Create(SAMPLER_TYPE::POINT_WRAP);

	// material
	auto& materialManager = rw.GetMaterialManager();
	auto materialID = materialManager.Create(fnv1a_64("ChunkMaterial"));
	materialManager.Get(materialID)->SetTexture(0, rw.GetRuntimeAtlas().GetShaderResourceView());
	materialManager.Get(materialID)->SetSampler(0, samplerManager.Get(samplerID)->Get());

	// Object Create
	m_pChunkObject = AddAndGetObject("TestObject #1");

	CTransform* const pTransform = m_pChunkObject->AddComponent<CTransform>();

	// Set Mesh Render
	CMeshRenderer* const pMeshRender = m_pChunkObject->AddComponent<CMeshRenderer>();
	pMeshRender->SetMesh(meshManager.Get(meshID));
	pMeshRender->SetPipeline(pipelineManager.Get(pipeID));
	pMeshRender->SetMaterial(materialManager.Get(materialID));
}