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
	_CreateWorldRender();

	m_VoxelWorld.Initialize(*this, m_pChunkPipeline, m_pChunkMaterial);

	m_blockBreakParticleSystem.Initialize(GetRenderWorld());

	m_pPlayer = AddAndGetObject("Player");
	auto* tr = m_pPlayer->AddComponent<CTransform>();
	tr->Init();
	tr->SetLocalTrans({ 0, 5.f, 0 });
	
	auto* ctrl = m_pPlayer->AddComponent<CPlayerController>();
	ctrl->Init();

	auto* motor = m_pPlayer->AddComponent<CCharacterMotor>();
	motor->Init();
	motor->SetWorld(&m_VoxelWorld);
	motor->SetFrozen(true);

	auto* inventory = m_pPlayer->AddComponent<CInventoryComponent>();
	inventory->Init();

	auto* interactor = m_pPlayer->AddComponent<CBlockInteractor>();
	interactor->Init();
	interactor->SetWorld(&m_VoxelWorld);
	interactor->SetParticleSystem(&m_blockBreakParticleSystem);

	// 카메라 피벗(자식 오브젝트)
	auto* pivot = AddAndGetObject("PlayerCameraPivot");
	pivot->SetParentID(m_pPlayer->GetID());

	auto* pivotTransform = pivot->AddComponent<CTransform>();
	pivotTransform->Init();

	auto* cam = pivot->AddComponent<CCamera>();
	cam->Init();
	m_pCurrentCamera = cam;

	_CreateHighlight();
	
	_CreateUICamera();
	_CreateCrosshairUI();

	ctrl->SetCameraTransform(pivotTransform);
	interactor->SetCameraTransform(pivotTransform);
	interactor->SetHighlightObject(m_pHighlightObject);

	_CreateSkyBillboardResources();

	m_bSpawnStreamingReady = false;
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

	timeParams = m_VoxelWorld.GetWorldTime().Evaluate();

#ifdef IMGUI_ACTIVATE
	ImGui::Text("Chunk Bounds (F2) : %s", m_bShowChunkBounds ? "ON" : "OFF");
	ImGui::Text("Force Active Chunks Reload! (F5)");

	ImGui::Text("Day01			: %.4f", timeParams.day01);
	ImGui::Text("TickOfDay		: %.1f", timeParams.tickOfDay);
	ImGui::Text("Daylight		: %.3f", timeParams.daylight);
	ImGui::Text("Night			: %.3f", timeParams.night);
	ImGui::Text("SunAngle		: %.3f", timeParams.sunAngleRad);
	ImGui::Text("MoonAngle		: %.3f", timeParams.moonAngleRad);

	ImGui::DragFloat("Sun size : %.3f", &m_fSunBillboardSize);
	ImGui::DragFloat("Moon size : %.3f", &m_fMoonBillboardSize);

	ImGui::DragFloat("Shadow Bias : %.6f", &m_debugBias, 0.00001f, 0.0f, 0.002f, "%.6f");

#endif // IMGUI_ACTIVATE

	if (CInputManager::Get().Keyboard().GetKeyUp(VK_F2))
	{
		m_bShowChunkBounds = !m_bShowChunkBounds;
	}

	if (CInputManager::Get().Keyboard().GetKeyUp(VK_F3))
	{
		m_eSectionDebugMode = static_cast<ESectionDebugMode>((static_cast<int>(m_eSectionDebugMode) + 1) % static_cast<int>(ESectionDebugMode::COUNT));
	}

	CTransform* tr = m_pPlayer->GetComponent<CTransform>();
	tr->BuildWorldMatrix();
	XMFLOAT3 Trans = tr->GetWorldTrans();

	m_VoxelWorld.Update(fDelta, Trans);
	m_blockBreakParticleSystem.Update(fDelta);

	if (CInputManager::Get().Keyboard().GetKeyUp(VK_F5))
	{
		m_VoxelWorld.GetChunkWorld().DebugRequestReloadActiveColumns();
	}

	if (!m_bSpawnStreamingReady)
	{
		m_bSpawnStreamingReady = m_VoxelWorld.GetChunkWorld().IsSpawnAreaReady(Trans);

		
		constexpr float kPlayerHalfWidth = 0.3f;
		constexpr float kPlayerHalfHeight = 0.9f;
		const int spawnWx = 0;
		const int spawnWz = 0;
		float spawnFootY = 0.f;

		if (m_VoxelWorld.FindSpawnFootY(spawnWx, spawnWz, { kPlayerHalfWidth, kPlayerHalfHeight, kPlayerHalfWidth }, spawnFootY))
		{
			tr->SetLocalTrans({ spawnWx, spawnFootY, spawnWz });
		}

		if (auto* motor = m_pPlayer->GetComponent<CCharacterMotor>())
		{
			motor->SetFrozen(!m_bSpawnStreamingReady);
		}
	}
}

void CTestScene::LateUpdate(float fDelta)
{
	CScene::LateUpdate(fDelta);

	m_VoxelWorld.LateUpdate(*this);
}

void CTestScene::BuildRenderFrame()
{
	CRenderWorld& rw = GetRenderWorld();

	const CCamera* pCurrentCamera = GetCurrentCamera();

	// matrix setting
	rw.SetViewMatrix(pCurrentCamera->GetViewMatrix());
	rw.SetProjectionMatrix(pCurrentCamera->GetProjMatrix());

	if (m_pUICamera)
	{
		m_pUICamera->UpdateCameraMatrix();
		rw.SetUIViewMatrix(m_pUICamera->GetViewMatrix());
		rw.SetUIProjectionMatrix(m_pUICamera->GetProjMatrix());
	}

	auto SnapToStep = [](float v, float step)
	{
		return floorf(v / step + 0.5f) * step;
	};

	{
		// sun / moon direction
		XMFLOAT3 sunDir{}, moonDir{};
		_CalcSunMoonDirection(sunDir, moonDir);

		// daylight 기반 조명 세기
		const float sunIntensity = saturate(timeParams.daylight * 1.15f);

		// 실제 lighting 방향도 sunDir와 통일
		rw.SetDirectionalLight(sunDir, { 1.0f, 0.97f, 0.92f }, sunIntensity);

		const XMFLOAT3 ambientNight = { 0.05f, 0.07f, 0.10f };
		const XMFLOAT3 ambientDay = { 0.28f, 0.30f, 0.33f };
		rw.SetAmbientLight(_LerpColor(ambientNight, ambientDay, timeParams.daylight));

		// shadow는 낮에만 유효하게
		const bool bShadowEnabled = (timeParams.daylight > 0.08f);

		// focus: player 기준 + world snap
		XMFLOAT3 focus = { 0.f, 0.f, 0.f };
		if (auto* playerTr = m_pPlayer->GetComponent<CTransform>())
		{
			playerTr->BuildWorldMatrix();
			focus = playerTr->GetWorldTrans();
		}

		// 지면 기준으로 고정
		//focus.y = 0.0f;

		// world-space snap
		focus.x = SnapToStep(focus.x, 0.5f);
		focus.z = SnapToStep(focus.z, 0.5f);

		XMVECTOR vFocus = XMLoadFloat3(&focus);
		XMVECTOR vLightDir = XMVector3Normalize(XMLoadFloat3(&sunDir));

		// directional light camera
		XMVECTOR vEye = vFocus + XMVectorScale(vLightDir, 48.0f);
		XMVECTOR vTarget = vFocus;

		XMVECTOR vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		float upDot = fabsf(XMVectorGetX(XMVector3Dot(vLightDir, vUp)));
		if (upDot > 0.98f)
			vUp = XMVectorSet(0.f, 0.f, 1.f, 0.f);

		const float kShadowOrthoSize = 48.0f;
		const float kShadowNear = 1.0f;
		const float kShadowFar = 120.0f;
		const float kShadowMapSize = 2048.0f; // CRenderWorld와 맞춰둘 것

		XMMATRIX matLightView = XMMatrixLookAtLH(vEye, vTarget, vUp);

		// light-space texel snap
		XMVECTOR vFocusLS = XMVector3TransformCoord(vFocus, matLightView);

		XMFLOAT3 focusLS{};
		XMStoreFloat3(&focusLS, vFocusLS);

		const float texelSize = kShadowOrthoSize / kShadowMapSize;

		const float snappedX = roundf(focusLS.x / texelSize) * texelSize;
		const float snappedY = roundf(focusLS.y / texelSize) * texelSize;

		XMVECTOR vDeltaLS = XMVectorSet(
			snappedX - focusLS.x,
			snappedY - focusLS.y,
			0.0f,
			0.0f
		);

		XMMATRIX invLightView = XMMatrixInverse(nullptr, matLightView);
		XMVECTOR vDeltaWS = XMVector3TransformNormal(vDeltaLS, invLightView);

		vEye += vDeltaWS;
		vTarget += vDeltaWS;

		matLightView = XMMatrixLookAtLH(vEye, vTarget, vUp);
		XMMATRIX matLightProj = XMMatrixOrthographicLH(
			kShadowOrthoSize,
			kShadowOrthoSize,
			kShadowNear,
			kShadowFar
		);

		rw.SetLightViewProj(matLightView * matLightProj);

		// 낮이 아니면 사실상 shadow 끄기
		rw.SetShadowParams(m_debugBias, bShadowEnabled ? 0.35f : 1.0f);
	}

	const CTransform* pCamTr = pCurrentCamera->GetTransform();
	XMFLOAT3 camPos = { 0.f, 0.f, 0.f };
	XMFLOAT3 camLook = { 0.f, 0.f, 0.f };

	if (pCamTr)
	{
		camPos = pCamTr->GetWorldTrans();
		camLook = pCamTr->GetLookNorm();
	}

	GetObjectManager().ForEachAliveEnabled([&](CObject& obj)
	{
		auto* render = obj.GetComponent<CMeshRenderer>();
		auto* transform = obj.GetComponent<CTransform>();

		if (!render || !transform) return;
		if (!render->GetMesh()) return;

		transform->BuildWorldMatrix();
		const XMMATRIX matWorld = transform->GetWorldMatrix();

		// shadow pass
		if (render->GetRenderPass() == ERenderPass::OPAQUE_PASS && m_pChunkShadowPipeline)
		{
			RenderItem shadowItem{};
			shadowItem.eRenderPass = ERenderPass::SHADOW_PASS;
			shadowItem.pMesh = render->GetMesh();
			shadowItem.pPipeline = m_pChunkShadowPipeline;
			shadowItem.pMaterial = nullptr;
			DirectX::XMStoreFloat4x4(&shadowItem.world, XMMatrixTranspose(matWorld));
			rw.Submit(shadowItem);
		}

		// main pass
		RenderItem item{};
		item.eRenderPass = render->GetRenderPass();
		item.pMesh = render->GetMesh();
		item.pPipeline = render->GetPipeline();
		item.pMaterial = render->GetMaterial();

		const XMFLOAT3 worldPos = transform->GetWorldTrans();
		const XMFLOAT3 toObj =
		{
			worldPos.x - camPos.x,
			worldPos.y - camPos.y,
			worldPos.z - camPos.z
		};

		item.fSortDepth =
			toObj.x * camLook.x +
			toObj.y * camLook.y +
			toObj.z * camLook.z;

		DirectX::XMStoreFloat4x4(&item.world, XMMatrixTranspose(matWorld));
		rw.Submit(item);
	});

	_SubmitSunMoonBillboards(rw);
	_ApplySkyClearColor();

	m_blockBreakParticleSystem.SubmitRender(rw, *pCurrentCamera);

	_SubmitChunkBoundsDebug(rw);
	_SubmitSectionBoundsDebug(rw);

	if (m_pCrosshairMesh && m_pCrosshairPipeline)
	{
		const float crosshairSize = 32.0f;

		XMMATRIX matWorld = XMMatrixScaling(crosshairSize, crosshairSize, 1.0f) * XMMatrixTranslation(0.f, 0.f, 0.f);

		RenderItem uiItem{};
		uiItem.eRenderPass = ERenderPass::ORTH_PASS;
		uiItem.pMesh = m_pCrosshairMesh;
		uiItem.pPipeline = m_pCrosshairPipeline;
		uiItem.pMaterial = m_pCrosshairMaterial;
		XMStoreFloat4x4(&uiItem.world, XMMatrixTranspose(matWorld));
		rw.Submit(uiItem);
	}

	GetObjectManager().ProcessPeddingDestroy();
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
	pipeline->CreateOpaqueState(rw.GetDevice());

	// mesh
	auto& meshManager = rw.GetMeshManager();
	auto highlightMeshID = meshManager.CreateAABBLine(fnv1a_64("AABBHighlight"));

	auto& materialManager = rw.GetMaterialManager();
	auto materialID = materialManager.Create(fnv1a_64("HighlightMaterial"));

	// debug chunk bounds 용으로도 같이 보관
	m_pChunkBoundsDebugMesh = meshManager.Get(highlightMeshID);
	m_pChunkBoundsDebugPipeline = pipeline;
	m_pChunkBoundsDebugMaterial = materialManager.Get(materialID);

	m_pHighlightObject = AddAndGetObject("BlockHighlight");
	auto* tr = m_pHighlightObject->AddComponent<CTransform>();
	auto* mr = m_pHighlightObject->AddComponent<CMeshRenderer>();

	mr->SetMesh(meshManager.Get(highlightMeshID));
	mr->SetPipeline(pipeline);
	mr->SetMaterial(materialManager.Get(materialID));
	mr->SetRenderPass(ERenderPass::DEBUG_PASS);

	m_pHighlightObject->SetEnable(false);
}

void CTestScene::_CreateWorldRender()
{
	CRenderWorld& rw = GetRenderWorld();

	auto& shaderManager = rw.GetShaderManager();
	auto& ilManager = rw.GetIALayoutManager();
	auto& pipelineManager = rw.GetPipelineManager();
	auto& samplerManager = rw.GetSamplerManager();
	auto& materialManager = rw.GetMaterialManager();

	// shader
	auto normalShaderID = fnv1a_64("NormalImageForward");
	auto normalShader = shaderManager.CreateShader(normalShaderID, 0);

	auto shadowShaderID = fnv1a_64("ShadowDepth");
	auto shadowShader = shaderManager.CreateShader(shadowShaderID, 0);

	shaderManager.Compile();

	// input layout	
	auto layoutID = ilManager.Create(VERTEX_POSITION_NORMAL_UV_COLOR::GetLayout(), { normalShaderID, 0 }, normalShader->GetVertexBlob());
	auto shadowLayoutID = ilManager.Create(VERTEX_POSITION_NORMAL_UV_COLOR::GetLayout(), { shadowShaderID, 0 }, shadowShader->GetVertexBlob());

	// opaque chunk pipeline
	auto pipeID = pipelineManager.Create(fnv1a_64("ChunkPipeline"));
	auto pipeline = pipelineManager.Get(pipeID);
	pipeline->SetShader(shaderManager.Get(normalShaderID, 0));
	pipeline->SetInputLayout(ilManager.Get(layoutID));
	pipeline->CreateOpaqueState(rw.GetDevice());
	pipeline->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// shadow chunk pipeline
	auto shadowPipeID = pipelineManager.Create(fnv1a_64("ChunkShadowPipeline"));
	auto shadowPipeline = pipelineManager.Get(shadowPipeID);
	shadowPipeline->SetShader(shaderManager.Get(shadowShaderID, 0));
	shadowPipeline->SetInputLayout(ilManager.Get(shadowLayoutID));
	shadowPipeline->CreateOpaqueState(rw.GetDevice());
	shadowPipeline->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// sampler	
	auto albedoSamplerID = samplerManager.Create(SAMPLER_TYPE::POINT_WRAP);
	auto shadowSamplerID = samplerManager.Create(SAMPLER_TYPE::SHADOWCOMPARISON);

	// material	
	auto materialID = materialManager.Create(fnv1a_64("ChunkMaterial"));
	auto* material = materialManager.Get(materialID);

	material->SetTexture(0, BlockResDB.GetAtlasTextureView());
	material->SetSampler(0, samplerManager.Get(albedoSamplerID)->Get());

	material->SetTexture(1, rw.GetShadowMapSRV());
	material->SetSampler(1, samplerManager.Get(shadowSamplerID)->Get());

	m_pChunkPipeline = pipeline;
	m_pChunkShadowPipeline = shadowPipeline;
	m_pChunkMaterial = material;
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

void CTestScene::_CreateUICamera()
{
	auto* uiCamObj = AddAndGetObject("UICamera");
	auto* tr = uiCamObj->AddComponent<CTransform>();
	tr->Init();
	tr->SetLocalTrans({0.f, 0.f, -10.f});
	tr->BuildWorldMatrix();

	auto* cam = uiCamObj->AddComponent<CCamera>();
	cam->Init();
	cam->SetProjToOrthographic();
	cam->SetOrthographicSize((float)g_ScreenSizeX, (float)g_ScreenSizeY);
	cam->SetOrthographicNearFar(0.1f, 1000.f);
	cam->UpdateCameraMatrix();

	m_pUICamera = cam;
}

void CTestScene::_CreateCrosshairUI()
{
	CRenderWorld& rw = GetRenderWorld();

	auto& shaderManager = rw.GetShaderManager();
	auto& ilManager = rw.GetIALayoutManager();
	auto& pipelineManager = rw.GetPipelineManager();
	auto& meshManager = rw.GetMeshManager();
	auto& materialManager = rw.GetMaterialManager();
	auto& textureManager = rw.GetTextureManager();
	auto& samplerManager = rw.GetSamplerManager();

	// shader
	const uint64_t shaderID = fnv1a_64("UIInvertMask");
	auto* shader = shaderManager.CreateShader(shaderID, 0);
	shaderManager.Compile();

	// input layout
	const uint64_t layoutID =
		ilManager.Create(VERTEX_POSITION_UV::GetLayout(), { shaderID, 0 }, shader->GetVertexBlob());

	// pipeline
	const uint64_t pipeID = pipelineManager.Create(fnv1a_64("UICrosshairPipeline"));
	auto* pipeline = pipelineManager.Get(pipeID);
	pipeline->SetShader(shaderManager.Get(shaderID, 0));
	pipeline->SetInputLayout(ilManager.Get(layoutID));
	pipeline->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pipeline->CreateUIInvertState(rw.GetDevice(), true);

	// mesh
	const uint64_t meshID = meshManager.CreateQuad(fnv1a_64("UICrosshairQuad"));

	// material
	const uint64_t materialID = materialManager.Create(fnv1a_64("UICrosshairMaterial"));
	auto* material = materialManager.Get(materialID);

	// texture
	const uint64_t textureID = textureManager.LoadTexture2D(
		fnv1a_64("ui/crosshair"),
		"../Resource/assets/minecraft/textures/gui/sprites/hud/crosshair.png",
		TEXTURE_USAGE::StaticColor
	);

	// sampler
	const uint64_t samplerID = samplerManager.Create(SAMPLER_TYPE::LINEAR_WARP);

	material->SetSampler(0, samplerManager.Get(samplerID)->Get());
	material->SetTexture(0, textureManager.GetTexture(textureID)->GetShaderResourceView());

	m_pCrosshairMesh = meshManager.Get(meshID);
	m_pCrosshairPipeline = pipeline;
	m_pCrosshairMaterial = material;
}

void CTestScene::_CreateSkyBillboardResources()
{
	CRenderWorld& rw = GetRenderWorld();
	auto& shaderManager = rw.GetShaderManager();
	auto& ilManager = rw.GetIALayoutManager();
	auto& pipelineManager = rw.GetPipelineManager();
	auto& meshManager = rw.GetMeshManager();
	auto& materialManager = rw.GetMaterialManager();
	auto& textureManager = rw.GetTextureManager();
	auto& samplerManager = rw.GetSamplerManager();

	const uint64_t shaderID = fnv1a_64("SkyBillboard");
	auto* shader = shaderManager.CreateShader(shaderID, 0);
	shaderManager.Compile();

	const uint64_t layoutID = ilManager.Create(VERTEX_POSITION_UV::GetLayout(), {shaderID, 0}, shader->GetVertexBlob());

	const uint64_t pipeID = pipelineManager.Create(fnv1a_64("BillbaordPipeline"));
	auto* pipeline = pipelineManager.Get(pipeID);
	pipeline->SetShader(shaderManager.Get(shaderID, 0));
	pipeline->SetInputLayout(ilManager.Get(layoutID));
	pipeline->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pipeline->CreateTransparentAlphaState(rw.GetDevice(), true);

	const uint64_t meshID = meshManager.CreateQuad(fnv1a_64("SkyBillboardQuad"));
	const uint64_t sunMatID = materialManager.Create(fnv1a_64("SunBillboardMaterial"));
	const uint64_t moonMatID = materialManager.Create(fnv1a_64("MoonBillboardMaterial"));

	m_pSkyBillboardMesh = meshManager.Get(meshID);
	m_pSkyBillboardPipeline = pipeline;
	m_pSunBillboardMaterial = materialManager.Get(sunMatID);
	m_pMoonBillboardMaterial = materialManager.Get(moonMatID);

	uint64_t sunTextureID = textureManager.LoadTexture2D(fnv1a_64("sun"), "../Resource/assets/minecraft/textures/environment/celestial/sun.png"
		, TEXTURE_USAGE::StaticColor);

	uint64_t moonTextureID = textureManager.LoadTexture2D(fnv1a_64("moon"), "../Resource/assets/minecraft/textures/environment/celestial/moon/full_moon.png"
		, TEXTURE_USAGE::StaticColor);

	auto samplerID = samplerManager.Create(SAMPLER_TYPE::POINT_WRAP);

	m_pSunBillboardMaterial->SetSampler(0, samplerManager.Get(samplerID)->Get());
	m_pSunBillboardMaterial->SetTexture(0, rw.GetTextureManager().GetTexture(sunTextureID)->GetShaderResourceView());

	m_pMoonBillboardMaterial->SetSampler(0, samplerManager.Get(samplerID)->Get());
	m_pMoonBillboardMaterial->SetTexture(0, rw.GetTextureManager().GetTexture(moonTextureID)->GetShaderResourceView());
}

void CTestScene::_SubmitSunMoonBillboards(CRenderWorld& rw)
{
	if (!m_bShowSunMoon)
		return;

	if (nullptr == m_pSkyBillboardMesh ||
		nullptr == m_pSkyBillboardPipeline ||
		nullptr == m_pSunBillboardMaterial ||
		nullptr == m_pMoonBillboardMaterial)
		return;

	const CCamera* pCam = GetCurrentCamera();
	if (nullptr == pCam || nullptr == pCam->GetTransform())
		return;

	const CTransform* pCamTr = pCam->GetTransform();

	const XMFLOAT3 camPos = pCamTr->GetWorldTrans();

	// 네 current transform 구현에 GetRight/GetUp이 있으면 이걸 쓰고,
	// 없으면 camera world matrix/inverse view에서 뽑아도 됨.
	const XMFLOAT3 camRight = pCamTr->GetRight();
	const XMFLOAT3 camUp = pCamTr->GetUp();

	XMFLOAT3 sunDir{}, moonDir{};
	_CalcSunMoonDirection(sunDir, moonDir);

	const XMFLOAT3 sunCenter =
	{
		camPos.x + sunDir.x * m_fSkyBillboardRadius,
		camPos.y + sunDir.y * m_fSkyBillboardRadius,
		camPos.z + sunDir.z * m_fSkyBillboardRadius,
	};

	const XMFLOAT3 moonCenter =
	{
		camPos.x + moonDir.x * m_fSkyBillboardRadius,
		camPos.y + moonDir.y * m_fSkyBillboardRadius,
		camPos.z + moonDir.z * m_fSkyBillboardRadius,
	};

	{
		const XMMATRIX matWorld = _BuildSkyLockedQuadWorld(sunCenter, sunDir, m_fSunBillboardSize, m_fSunBillboardSize);

		RenderItem item{};
		item.eRenderPass = ERenderPass::TRANSPARENT_PASS;
		item.pMesh = m_pSkyBillboardMesh;
		item.pPipeline = m_pSkyBillboardPipeline;
		item.pMaterial = m_pSunBillboardMaterial;
		XMStoreFloat4x4(&item.world, XMMatrixTranspose(matWorld));
		rw.Submit(item);
	}

	{
		const XMMATRIX matWorld = _BuildSkyLockedQuadWorld(moonCenter, moonDir, m_fMoonBillboardSize, m_fMoonBillboardSize);

		RenderItem item{};
		item.eRenderPass = ERenderPass::TRANSPARENT_PASS;
		item.pMesh = m_pSkyBillboardMesh;
		item.pPipeline = m_pSkyBillboardPipeline;
		item.pMaterial = m_pMoonBillboardMaterial;
		XMStoreFloat4x4(&item.world, XMMatrixTranspose(matWorld));
		rw.Submit(item);
	}

}

XMFLOAT3 CTestScene::_LerpColor(const XMFLOAT3& a, const XMFLOAT3& b, float t)
{
	t = saturate(t);

	return XMFLOAT3(
		a.x + (b.x - a.x) * t,
		a.y + (b.y - a.y) * t,
		a.z + (b.z - a.z) * t
	);
}

void CTestScene::_ApplySkyClearColor()
{
	const WorldTimeParams time = m_VoxelWorld.GetWorldTime().Evaluate();

	const float day01 = time.day01;
	const float daylight = time.daylight;
	const float night = time.night;

	// 1차 실험용 색
	const XMFLOAT3 kNightColor = { 0.02f, 0.04f, 0.10f };
	const XMFLOAT3 kDayColor = { 0.45f, 0.70f, 0.98f };
	const XMFLOAT3 kDawnColor = { 0.95f, 0.46f, 0.18f };

	// 기본은 밤 <-> 낮 보간
	XMFLOAT3 sky = _LerpColor(kNightColor, kDayColor, daylight);

	// sunrise / sunset 강조
	const float sunrise = pulse(day01, 0.25f, 0.10f);
	const float sunset = pulse(day01, 0.75f, 0.10f);
	const float dusk = std::max(sunrise, sunset);

	// 해 뜨고 질 때만 주황색을 얹음
	sky = _LerpColor(sky, kDawnColor, dusk * 0.65f);

	// 아주 깊은 밤은 조금 더 눌러줌
	const float deepNight = smooth(night);
	sky.x *= (1.f - 0.15f * deepNight);
	sky.y *= (1.f - 0.10f * deepNight);
	sky.z *= (1.f - 0.05f * deepNight);

	GetRenderWorld().SetSkyColor(sky);
	GetRenderWorld().SetBackColor(sky.x, sky.y, sky.z, 1.0f);
}

XMMATRIX CTestScene::_BuildSkyLockedQuadWorld(
	const XMFLOAT3& center,
	const XMFLOAT3& dirFromCam,   // sunDir or moonDir
	float width,
	float height)
{ // 카메라 회전에는 반응안하도록
	XMVECTOR vDir = XMVector3Normalize(XMLoadFloat3(&dirFromCam));

	// quad가 sky sphere 중심(=카메라) 쪽을 보게
	XMVECTOR vNormal = vDir;

	XMVECTOR vWorldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	// 극점 근처에서 cross가 불안정해지는 것 방지
	float d = XMVectorGetX(XMVector3Dot(vNormal, vWorldUp));
	if (fabsf(d) > 0.98f)
	{
		vWorldUp = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	}

	XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vWorldUp, vNormal));
	XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vNormal, vRight));

	vRight *= (width * 0.5f);
	vUp *= (height * 0.5f);

	XMFLOAT3 right{}, up{}, normal{};
	XMStoreFloat3(&right, vRight);
	XMStoreFloat3(&up, vUp);
	XMStoreFloat3(&normal, vNormal);

	return XMMATRIX(
		right.x, right.y, right.z, 0.f,
		up.x, up.y, up.z, 0.f,
		normal.x, normal.y, normal.z, 0.f,
		center.x, center.y, center.z, 1.f
	);
}

XMMATRIX CTestScene::_BuildScreenAlignedBillboardWorld(const XMFLOAT3& center, const XMFLOAT3& camRight, const XMFLOAT3& camUp, float width, float height)
{
	XMVECTOR vRight = XMVector3Normalize(XMLoadFloat3(&camRight)) * (width * 0.5f);
	XMVECTOR vUp = XMVector3Normalize(XMLoadFloat3(&camUp)) * (height * 0.5f);

	XMFLOAT3 right{}, up{};
	XMStoreFloat3(&right, vRight);
	XMStoreFloat3(&up, vUp);

	XMVECTOR vForward = XMVector3Normalize(XMVector3Cross(vRight, vUp));
	XMFLOAT3 forward{};
	XMStoreFloat3(&forward, vForward);

	return XMMATRIX(
		right.x, right.y, right.z, 0.0f,
		up.x, up.y, up.z, 0.0f,
		forward.x, forward.y, forward.z, 0.0f,
		center.x, center.y, center.z, 1.0f
	);
}

void CTestScene::_CalcSunMoonDirection(XMFLOAT3& outSunDir, XMFLOAT3& outMoonDir) const
{
	const float a = timeParams.sunAngleRad;

	outSunDir =
	{
		0.0f,
		sinf(a),
		cosf(a)
	};

	outMoonDir =
	{
		-outSunDir.x,
		-outSunDir.y,
		-outSunDir.z
	};

	XMVECTOR s = XMVector3Normalize(XMLoadFloat3(&outSunDir));
	XMVECTOR m = XMVector3Normalize(XMLoadFloat3(&outMoonDir));
	XMStoreFloat3(&outSunDir, s);
	XMStoreFloat3(&outMoonDir, m);
}

void CTestScene::_SubmitChunkBoundsDebug(CRenderWorld& rw) const
{
	if (!m_bShowChunkBounds)
		return;

	if (nullptr == m_pChunkBoundsDebugMesh || nullptr == m_pChunkBoundsDebugPipeline || nullptr == m_pChunkBoundsDebugMaterial)
		return;

	const CChunkWorld& chunkWorld = m_VoxelWorld.GetChunkWorld();

	chunkWorld.ForEachActiveColumn([&](const CChunkColumn& column)
	{
		const ChunkCoord& coord = column.GetCoord();

		const float worldX = static_cast<float>(coord.x * CHUNK_SIZE_X);
		const float worldZ = static_cast<float>(coord.z * CHUNK_SIZE_Z);

		XMMATRIX matScale = XMMatrixScaling(
			static_cast<float>(CHUNK_SIZE_X),
			static_cast<float>(CHUNK_SIZE_Y),
			static_cast<float>(CHUNK_SIZE_Z));

		XMMATRIX matTrans = XMMatrixTranslation(worldX, 0.0f, worldZ);
		XMMATRIX matWorld = matScale * matTrans;

		RenderItem item{};
		item.eRenderPass = ERenderPass::DEBUG_PASS;
		item.pMesh = m_pChunkBoundsDebugMesh;
		item.pPipeline = m_pChunkBoundsDebugPipeline;
		item.pMaterial = m_pChunkBoundsDebugMaterial;

		DirectX::XMStoreFloat4x4(&item.world, XMMatrixTranspose(matWorld));
		rw.Submit(item);
	});
}

void CTestScene::_SubmitSectionBoundsDebug(CRenderWorld& rw) const
{
	if (m_eSectionDebugMode == ESectionDebugMode::OFF)
		return;

	if (nullptr == m_pChunkBoundsDebugMesh ||
		nullptr == m_pChunkBoundsDebugPipeline ||
		nullptr == m_pChunkBoundsDebugMaterial)
		return;

	const CChunkWorld& chunkWorld = m_VoxelWorld.GetChunkWorld();

	chunkWorld.ForEachActiveColumn([&](const CChunkColumn& column)
	{
		const ChunkCoord& coord = column.GetCoord();

		const float worldX = static_cast<float>(coord.x * CHUNK_SIZE_X);
		const float worldZ = static_cast<float>(coord.z * CHUNK_SIZE_Z);

		for (int sy = 0; sy < CHUNK_SECTION_COUNT; ++sy)
		{
			bool bDraw = false;

			switch (m_eSectionDebugMode)
			{
				case ESectionDebugMode::EXIST_ONLY:
				{
					bDraw = (column.GetSection(sy) != nullptr);
				} break;

				case ESectionDebugMode::ALL:
				{
					bDraw = true;
				} break;
				default: break;
			}

			if (!bDraw)
				continue;

			const float worldY = static_cast<float>(sy * CHUNK_SECTION_SIZE);

			XMMATRIX matScale = XMMatrixScaling(
				static_cast<float>(CHUNK_SIZE_X),
				static_cast<float>(CHUNK_SECTION_SIZE),
				static_cast<float>(CHUNK_SIZE_Z));

			XMMATRIX matTrans = XMMatrixTranslation(worldX, worldY, worldZ);
			XMMATRIX matWorld = matScale * matTrans;

			RenderItem item{};
			item.eRenderPass = ERenderPass::DEBUG_PASS;
			item.pMesh = m_pChunkBoundsDebugMesh;
			item.pPipeline = m_pChunkBoundsDebugPipeline;
			item.pMaterial = m_pChunkBoundsDebugMaterial;

			DirectX::XMStoreFloat4x4(&item.world, XMMatrixTranspose(matWorld));
			rw.Submit(item);
		}
	});
}
