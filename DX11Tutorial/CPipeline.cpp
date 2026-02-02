#include "pch.h"
#include "CPipeline.h"

void CPipeline::Init(uint64_t shaderID_, uint32_t shaderFlags_, uint32_t uInputLayoutID_)
{
}

void CPipeline::Begin(ID3D11DeviceContext* pDeviceContext_)
{
	pDeviceContext_->RSSetState(m_pRasterizerState.Get());
	pDeviceContext_->OMSetBlendState(m_pBlendState.Get(), nullptr, 0xffffffff);
	pDeviceContext_->OMSetDepthStencilState(m_pDepthStencilState.Get(), 0);
	//pDeviceContext_->PSSetSamplers(0, m_vecSamplerStates.size(), m_vecSamplerStates.data()->GetAddressOf());
}

void CPipeline::End(ID3D11DeviceContext* pDeviceContext_)
{
}
