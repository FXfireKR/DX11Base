#include "pch.h"
#include "CInputLayerManager.h"

uint32_t CInputLayerManager::Create(const VertexLayoutDesc& layoutDesc_, const ShaderKey& shaderKey_)
{
    // duplicate check
    for (size_t i = 0; i < m_vecInputLayoutData.size(); ++i) {
        const auto& elem = m_vecInputLayoutData[i];
        if (elem.layoutDesc == layoutDesc_ && elem.shaderKey == shaderKey_) {
            return static_cast<uint32_t>(i);
        }
    }

    InputLayoutData newData;
    newData.layoutDesc = layoutDesc_;
    newData.shaderKey = shaderKey_;
    newData.data = make_unique<CInputLayer>();
    m_vecInputLayoutData.push_back(std::move(newData));
    return static_cast<uint32_t>(m_vecInputLayoutData.size() - 1);
}

const CInputLayer* CInputLayerManager::GetData(uint32_t id_) const
{
    if (id_ < m_vecInputLayoutData.size()) {
        return m_vecInputLayoutData[id_].data.get();
    }
    return nullptr;
}