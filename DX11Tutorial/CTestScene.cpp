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
	_CreateTextureAtlas();
	_CreateWorldRender();

	m_VoxelWorld.Initialize(*this, m_pChunkPipeline, m_pChunkMaterial);

	auto* player = AddAndGetObject("Player");
	auto* tr = player->AddComponent<CTransform>();
	tr->Init();
	tr->SetLocalTrans({ 0, 5.f, 0 });
	
	auto* ctrl = player->AddComponent<CPlayerController>();  
	ctrl->Init();

	auto* motor = player->AddComponent<CCharacterMotor>();
	motor->Init();
	motor->SetWorld(&m_VoxelWorld);

	auto* inventory = player->AddComponent<CInventoryComponent>();
	inventory->Init();

	auto* interactor = player->AddComponent<CBlockInteractor>();
	interactor->Init();
	interactor->SetWorld(&m_VoxelWorld);

	// 카메라 피벗(자식 오브젝트)
	auto* pivot = AddAndGetObject("PlayerCameraPivot");
	pivot->SetParentID(player->GetID());

	auto* pivotTransform = pivot->AddComponent<CTransform>();
	pivotTransform->Init();

	auto* cam = pivot->AddComponent<CCamera>();
	cam->Init();
	m_pCurrentCamera = cam;

	_CreateHighlight();

	ctrl->SetCameraTransform(pivotTransform);
	interactor->SetCameraTransform(pivotTransform);
	interactor->SetHighlightObject(m_pHighlightObject);
}

void CTestScene::Start()
{
	CScene::Start();
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
}

void CTestScene::LateUpdate(float fDelta)
{
	CScene::LateUpdate(fDelta);

	CChunkMesherSystem::RebuildDirtyChunks(*this, m_VoxelWorld.GetChunkWorld());

	//// DDA-Raycast
	//if (nullptr != m_pCurrentCamera) 
	//{
	//	XMFLOAT3 rayO{}, rayD{};
	//	_MakeCenterRay(*m_pCurrentCamera, rayO, rayD);

	//	BlockHitResult hit{};
	//	const float maxDist = 15.0f;

	//	m_pHighlightObject->SetEnable(false);

	//	if (true == m_VoxelWorld.RaycastBlock(rayO, rayD, maxDist, hit))
	//	{
	//		m_pHighlightObject->SetEnable(true);

	//		CTransform* tr = m_pHighlightObject->GetComponent<CTransform>();
	//		tr->SetLocalTrans({ (float)hit.block.x - 0.001f, (float)hit.block.y - 0.001f, (float)hit.block.z - 0.001f });
	//		tr->SetLocalScale({ 1.002f, 1.002f, 1.002f });
	//	}
	//}
}

void CTestScene::BuildRenderFrame()
{
	CRenderWorld& rw = GetRenderWorld();

	// matrix setting
	rw.SetViewMatrix(GetCurrentCamera()->GetViewMatrix());
	rw.SetProjectionMatrix(GetCurrentCamera()->GetProjMatrix());

	GetObjectManager().ForEachAliveEnabled([&](CObject& obj)
	{
		auto* render = obj.GetComponent<CMeshRenderer>();
		auto* transform = obj.GetComponent<CTransform>();

		if (!render || !transform) return;
		if (!render->GetMesh()) return;

		RenderItem item{};
		item.pMesh = render->GetMesh();
		item.pPipeline = render->GetPipeline();
		item.pMaterial = render->GetMaterial();

		XMStoreFloat4x4(&item.world, XMMatrixTranspose(transform->GetWorldMatrix()));
		rw.Submit(item);
	});
}

void CTestScene::_CreateChunkObject()
{
	//CRenderWorld& rw = GetRenderWorld();

	//RuntimeAtlasDesc ad{};
	//ad.width = 256;
	//ad.height = 256;
	//ad.tilePx = 16;
	//ad.eFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	//ad.bSRGB = true;
	//ad.bMipmap = false;

	//rw.GetRuntimeAtlas().Create(rw.GetDevice(), ad);

	//rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("bedrock"), "../Resource/assets/minecraft/textures/block/bedrock.png");
	//rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("bricks"), "../Resource/assets/minecraft/textures/block/bricks.png");
	//rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("glass"), "../Resource/assets/minecraft/textures/block/glass.png");
	//rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("gravel"), "../Resource/assets/minecraft/textures/block/gravel.png");
	//rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("sand"), "../Resource/assets/minecraft/textures/block/sand.png");
	//rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("stone"), "../Resource/assets/minecraft/textures/block/stone.png");

	//// shader
	//auto& shaderManager = rw.GetShaderManager();
	//auto shaderID = fnv1a_64("NormalImageForward");
	//auto shader = shaderManager.CreateShader(shaderID, 0);

	//shaderManager.Compile();

	//// input layout
	//auto& ilManager = rw.GetIALayoutManager();
	//auto layoutID = ilManager.Create(VERTEX_POSITION_UV_NORMAL::GetLayout(), { shaderID, 0 }, shader->GetVertexBlob());

	//// pipeline
	//auto& pipelineManager = rw.GetPipelineManager();
	//auto pipeID = pipelineManager.Create(fnv1a_64("ChunkPipeline"));
	//auto pipeline = pipelineManager.Get(pipeID);

	//pipeline->SetShader(shaderManager.Get(shaderID, 0));
	//pipeline->SetInputLayout(ilManager.Get(layoutID));
	//pipeline->CreateRaster(rw.GetDevice());

	//// sampler
	//auto& samplerManager = rw.GetSamplerManager();
	//auto samplerID = samplerManager.Create(SAMPLER_TYPE::POINT_WRAP);

	//// material
	//auto& materialManager = rw.GetMaterialManager();
	//auto materialID = materialManager.Create(fnv1a_64("ChunkMaterial"));
	//materialManager.Get(materialID)->SetTexture(0, rw.GetRuntimeAtlas().GetShaderResourceView());
	//materialManager.Get(materialID)->SetSampler(0, samplerManager.Get(samplerID)->Get());

	//m_pChunkPipeline = pipelineManager.Get(pipeID);
	//m_pChunkMaterial = materialManager.Get(materialID);

	//m_pChunkObject = AddAndGetObject("Chunk_0_0_0");
	//auto* tr = m_pChunkObject->AddComponent<CTransform>();
	//tr->SetLocalTrans({ 0.f, 0.f, 0.f });

	//auto* mr = m_pChunkObject->AddComponent<CMeshRenderer>();
	//mr->SetPipeline(m_pChunkPipeline);
	//mr->SetMaterial(m_pChunkMaterial);

	//auto* cnk = m_pChunkObject->AddComponent<CChunkComponent>();
	//cnk->Init();
	//cnk->SetMeshRenderer(mr);
	//cnk->SetChunkCoord({ 0,0,0 });

	//for (int z = 0; z < CHUNK_SIZE_Z; ++z)
	//	for (int y = 0; y < CHUNK_SIZE_Y; ++y)
	//		for (int x = 0; x < CHUNK_SIZE_X; ++x)
	//			cnk->SetBlock(x, y, z, rand() % 7);
}

void CTestScene::_CreateBaked()
{
	//CRenderWorld& rw = GetRenderWorld();

	//RuntimeAtlasDesc ad{};
	//ad.width = 256;
	//ad.height = 256;
	//ad.tilePx = 16;
	//ad.eFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	//ad.bSRGB = true;
	//ad.bMipmap = false;

	//rw.GetRuntimeAtlas().Create(rw.GetDevice(), ad);

	//rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("block/grass_block_snow"), "../Resource/assets/minecraft/textures/block/grass_block_snow.png");
	//rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("block/grass_block_side"), "../Resource/assets/minecraft/textures/block/grass_block_side.png");
	//rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("block/grass_block_side_overlay"), "../Resource/assets/minecraft/textures/block/grass_block_side_overlay.png");
	//rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("block/grass_block_top"), "../Resource/assets/minecraft/textures/block/grass_block_top.png");
	//rw.GetRuntimeAtlas().AddTileFromFile(rw.GetContext(), fnv1a_64("block/dirt"), "../Resource/assets/minecraft/textures/block/dirt.png");

	//// shader
	//auto& shaderManager = rw.GetShaderManager();
	//auto shaderID = fnv1a_64("NormalImageForward");
	//auto shader = shaderManager.CreateShader(shaderID, 0);

	//shaderManager.Compile();

	//// input layout
	//auto& ilManager = rw.GetIALayoutManager();
	//auto layoutID = ilManager.Create(VERTEX_POSITION_UV_NORMAL::GetLayout(), { shaderID, 0 }, shader->GetVertexBlob());

	//// pipeline
	//auto& pipelineManager = rw.GetPipelineManager();
	//auto pipeID = pipelineManager.Create(fnv1a_64("TrianglePipeline"));

	//auto pipeline = pipelineManager.Get(pipeID);

	//pipeline->SetShader(shaderManager.Get(shaderID, 0));
	//pipeline->SetInputLayout(ilManager.Get(layoutID));

	//// dummy
	//pipeline->CreateRaster(rw.GetDevice());

	//// mesh
	//auto& meshManager = rw.GetMeshManager();

	//BlockPropHashMap props;
	//props[fnv1a_64("snowy")] = fnv1a_64("false");

	//BLOCK_ID blockID = fnv1a_64("minecraft:grass_block");

	//STATE_INDEX sidx;
	//bool ok = BlockDB.EncodeStateIndex(blockID, props, sidx);
	//assert(ok);

	//std::vector<AppliedModel> outModels;
	//BlockDB.GetAppliedModels(blockID, sidx, outModels);
	//auto it = outModels.front();
	//auto meshID = meshManager.CreateMeshFromBakedModel(BlockDB.FindModelID(it.modelHash), rw.GetRuntimeAtlas());

	//// sampler
	//auto& samplerManager = rw.GetSamplerManager();
	//auto samplerID = samplerManager.Create(SAMPLER_TYPE::POINT_WRAP);

	//// material
	//auto& materialManager = rw.GetMaterialManager();
	//auto materialID = materialManager.Create(fnv1a_64("ChunkMaterial"));
	//materialManager.Get(materialID)->SetTexture(0, rw.GetRuntimeAtlas().GetShaderResourceView());
	//materialManager.Get(materialID)->SetSampler(0, samplerManager.Get(samplerID)->Get());

	//// Object Create
	//m_pChunkObject = AddAndGetObject("TestObject #1");

	//CTransform* const pTransform = m_pChunkObject->AddComponent<CTransform>();

	//// Set Mesh Render
	//CMeshRenderer* const pMeshRender = m_pChunkObject->AddComponent<CMeshRenderer>();
	//pMeshRender->SetMesh(meshManager.Get(meshID));
	//pMeshRender->SetPipeline(pipelineManager.Get(pipeID));
	//pMeshRender->SetMaterial(materialManager.Get(materialID));
}

void CTestScene::_MakeCenterRay(IN const CCamera& cam, OUT XMFLOAT3& orig, OUT XMFLOAT3& dir)
{
	orig = cam.GetTransform()->GetWorldTrans();
	dir = cam.GetTransform()->GetLook();

	XMVECTOR d = XMLoadFloat3(&dir);
	d = XMVector3Normalize(d);
	XMStoreFloat3(&dir, d);
}

void CTestScene::_CreateHighlight()
{
	CRenderWorld& rw = GetRenderWorld();

	// shader
	auto& shaderManager = rw.GetShaderManager();
	auto shaderID = fnv1a_64("Highlight");
	auto shader = shaderManager.CreateShader(shaderID, 0);

	shaderManager.Compile();

	// input layout
	auto& ilManager = rw.GetIALayoutManager();
	auto layoutID = ilManager.Create(VERTEX_POSITION::GetLayout(), { shaderID, 0 }, shader->GetVertexBlob());

	// pipeline
	auto& pipelineManager = rw.GetPipelineManager();
	auto pipeID = pipelineManager.Create(fnv1a_64("HighlightPipeline"));

	auto pipeline = pipelineManager.Get(pipeID);

	pipeline->SetShader(shaderManager.Get(shaderID, 0));
	pipeline->SetInputLayout(ilManager.Get(layoutID));
	pipeline->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// dummy
	pipeline->CreateRaster(rw.GetDevice());

	// mesh
	auto& meshManager = rw.GetMeshManager();
	auto highlightMeshID = meshManager.CreateAABBLine(fnv1a_64("AABBHighlight"));

	auto& materialManager = rw.GetMaterialManager();
	auto materialID = materialManager.Create(fnv1a_64("HighlightMaterial"));

	m_pHighlightObject = AddAndGetObject("BlockHighlight");
	auto* tr = m_pHighlightObject->AddComponent<CTransform>();
	auto* mr = m_pHighlightObject->AddComponent<CMeshRenderer>();

	mr->SetMesh(meshManager.Get(highlightMeshID));
	mr->SetPipeline(pipeline);
	mr->SetMaterial(materialManager.Get(materialID));

	m_pHighlightObject->SetEnable(false);
}

void CTestScene::_CreateWorldRender()
{
	//CRenderWorld& rw = GetRenderWorld();

	//// shader
	//auto& shaderManager = rw.GetShaderManager();
	//auto shaderID = fnv1a_64("NormalImageForward");
	//auto shader = shaderManager.CreateShader(shaderID, 0);

	//shaderManager.Compile();

	//// input layout
	//auto& ilManager = rw.GetIALayoutManager();
	//auto layoutID = ilManager.Create(VERTEX_POSITION_UV_NORMAL::GetLayout(), { shaderID, 0 }, shader->GetVertexBlob());

	//// pipeline
	//auto& pipelineManager = rw.GetPipelineManager();
	//auto pipeID = pipelineManager.Create(fnv1a_64("ChunkPipeline"));
	//auto pipeline = pipelineManager.Get(pipeID);

	//pipeline->SetShader(shaderManager.Get(shaderID, 0));
	//pipeline->SetInputLayout(ilManager.Get(layoutID));
	//pipeline->CreateRaster(rw.GetDevice());
	//pipeline->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//// sampler
	//auto& samplerManager = rw.GetSamplerManager();
	//auto samplerID = samplerManager.Create(SAMPLER_TYPE::POINT_WRAP);

	//// material
	//auto& materialManager = rw.GetMaterialManager();
	//auto materialID = materialManager.Create(fnv1a_64("ChunkMaterial"));

	//materialManager.Get(materialID)->SetTexture(0, rw.GetRuntimeAtlas().GetShaderResourceView());
	//materialManager.Get(materialID)->SetSampler(0, samplerManager.Get(samplerID)->Get());

	//m_pChunkPipeline = pipelineManager.Get(pipeID);
	//m_pChunkMaterial = materialManager.Get(materialID);
}

void CTestScene::_CreateTextureAtlas()
{
	/*CRenderWorld& rw = GetRenderWorld();
	CRuntimeAtlas& rta = rw.GetRuntimeAtlas();

	RuntimeAtlasDesc ad{};
	ad.width = 256;
	ad.height = 256;
	ad.tilePx = 16;
	ad.eFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	ad.bSRGB = true;
	ad.bMipmap = false;

	rta.Create(rw.GetDevice(), ad);

	rta.AddTileFromFile(rw.GetContext(), fnv1a_64("minecraft:block/grass_block_snow"), "../Resource/assets/minecraft/textures/block/grass_block_snow.png");
	rta.AddTileFromFile(rw.GetContext(), fnv1a_64("minecraft:block/grass_block_side"), "../Resource/assets/minecraft/textures/block/grass_block_side.png");
	rta.AddTileFromFile(rw.GetContext(), fnv1a_64("minecraft:block/grass_block_side_overlay"), "../Resource/assets/minecraft/textures/block/grass_block_side_overlay.png");
	rta.AddTileFromFile(rw.GetContext(), fnv1a_64("minecraft:block/grass_block_top"), "../Resource/assets/minecraft/textures/block/grass_block_top.png");
	rta.AddTileFromFile(rw.GetContext(), fnv1a_64("minecraft:block/dirt"), "../Resource/assets/minecraft/textures/block/dirt.png");
	rta.AddTileFromFile(rw.GetContext(), fnv1a_64("minecraft:block/stone"), "../Resource/assets/minecraft/textures/block/stone.png");
	rta.AddTileFromFile(rw.GetContext(), fnv1a_64("minecraft:block/sand"), "../Resource/assets/minecraft/textures/block/sand.png");
	rta.AddTileFromFile(rw.GetContext(), fnv1a_64("minecraft:block/bricks"), "../Resource/assets/minecraft/textures/block/bricks.png");*/
}
