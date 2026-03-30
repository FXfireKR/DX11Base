#include "pch.h"
#include "CBlockCrackRenderer.h"

#include "CRenderWorld.h"
#include "CBlockInteractor.h"
#include "CMeshManager.h"
#include "CShaderManager.h"
#include "CInputLayerManager.h"
#include "CPipelineManager.h"
#include "CMaterialManager.h"
#include "CTextureManager.h"
#include "CSamplerManager.h"

bool CBlockCrackRenderer::Initialize(CRenderWorld& rw)
{
    auto& shaderManager = rw.GetShaderManager();
    auto& ilManager = rw.GetIALayoutManager();
    auto& pipelineManager = rw.GetPipelineManager();
    auto& meshManager = rw.GetMeshManager();
    auto& materialManager = rw.GetMaterialManager();
    auto& textureManager = rw.GetTextureManager();
    auto& samplerManager = rw.GetSamplerManager();

    const uint64_t shaderID = fnv1a_64("NormalImageForward");
    const CShader* pShader = shaderManager.CreateShader(shaderID, 0);
    shaderManager.Compile();

    const uint64_t layoutID =
        ilManager.Create(VERTEX_POSITION_NORMAL_UV_COLOR::GetLayout(), { shaderID, 0 }, pShader->GetVertexBlob());

    const uint64_t pipeID = pipelineManager.Create(fnv1a_64("BlockCrackPipeline"));
    m_pPipeline = pipelineManager.Get(pipeID);
    m_pPipeline->SetShader(shaderManager.Get(shaderID, 0));
    m_pPipeline->SetInputLayout(ilManager.Get(layoutID));
    m_pPipeline->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pPipeline->CreateTransparentAlphaState(rw.GetDevice(), true);

    const uint64_t meshID = meshManager.CreateCube(fnv1a_64("BlockCrackCube"));
    m_pCubeMesh = meshManager.Get(meshID);

    const uint64_t crackSamplerID = samplerManager.Create(SAMPLER_TYPE::POINT_CLAMP);
    const uint64_t shadowSamplerID = samplerManager.Create(SAMPLER_TYPE::SHADOWCOMPARISON);

    for (int i = 0; i < 10; ++i)
    {
        char matName[64];
        sprintf_s(matName, "BlockCrackMaterial_%d", i);
        const uint64_t materialID = materialManager.Create(fnv1a_64(matName));
        CMaterial* pMaterial = materialManager.Get(materialID);

        char texPath[256];
        sprintf_s(texPath, "../Resource/assets/minecraft/textures/block/destroy_stage_%d.png", i);

        char texName[64];
        sprintf_s(texName, "destroy_stage_%d", i);
        const uint64_t textureID = textureManager.LoadTexture2D(
            fnv1a_64(texName),
            texPath,
            TEXTURE_USAGE::StaticColor);

        pMaterial->SetTexture(0, textureManager.GetTexture(textureID)->GetShaderResourceView());
        pMaterial->SetSampler(0, samplerManager.Get(crackSamplerID)->Get());

        // forward shader 재사용용 shadow map 바인딩
        pMaterial->SetTexture(1, rw.GetShadowMapSRV());
        pMaterial->SetSampler(1, samplerManager.Get(shadowSamplerID)->Get());

        m_arrStageMaterials[i] = pMaterial;
    }

    return (m_pCubeMesh != nullptr && m_pPipeline != nullptr);
}

void CBlockCrackRenderer::Update(const CBlockInteractor& interactor)
{
    m_bVisible = false;
    m_iStage = 0;

    if (!interactor.IsMining())
        return;

    const BlockCell& cell = interactor.GetMiningCell();
    if (cell.IsAir())
        return;

    if (!BlockDB.IsFullCube(cell.blockID))
        return;

    m_block = interactor.GetMiningBlock();

    const float t = interactor.GetBreakProgress01();
    m_iStage = std::clamp(static_cast<int>(t * 10.0f), 0, 9);
    m_bVisible = true;
}

void CBlockCrackRenderer::Submit(CRenderWorld& rw)
{
    if (!m_bVisible || !m_pCubeMesh || !m_pPipeline)
        return;

    CMaterial* pMaterial = m_arrStageMaterials[m_iStage];
    if (!pMaterial)
        return;

    const float eps = 0.001f;
    const float scale = 1.002f;

    const XMMATRIX matS = XMMatrixScaling(scale, scale, scale);
    const XMMATRIX matT = XMMatrixTranslation(
        static_cast<float>(m_block.x) - eps,
        static_cast<float>(m_block.y) - eps,
        static_cast<float>(m_block.z) - eps);

    RenderItem item{};
    item.eRenderPass = ERenderPass::TRANSPARENT_PASS;
    item.pMesh = m_pCubeMesh;
    item.pPipeline = m_pPipeline;
    item.pMaterial = pMaterial;
    XMStoreFloat4x4(&item.world, XMMatrixTranspose(matS * matT));
    rw.Submit(item);
}