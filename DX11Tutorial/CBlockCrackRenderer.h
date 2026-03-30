#pragma once
#include "ChunkTypes.h"

class CRenderWorld;
class CBlockInteractor;
class CMesh;
class CPipeline;
class CMaterial;

class CBlockCrackRenderer
{
public:
    CBlockCrackRenderer() = default;
    ~CBlockCrackRenderer() = default;

    bool Initialize(CRenderWorld& rw);
    void Update(const CBlockInteractor& interactor);
    void Submit(CRenderWorld& rw);

private:
    CMesh* m_pCubeMesh = nullptr;
    CPipeline* m_pPipeline = nullptr;
    std::array<CMaterial*, 10> m_arrStageMaterials{};

    bool m_bVisible = false;
    XMINT3 m_block{};
    int m_iStage = 0;
};