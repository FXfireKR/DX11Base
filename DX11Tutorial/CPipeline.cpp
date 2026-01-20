#include "pch.h"
#include "CPipeline.h"

CPipeline::CPipeline()
{
}

CPipeline::~CPipeline()
{
}

void CPipeline::Begin(ID3D11DeviceContext* pDeviceContext_)
{
	pDeviceContext_->RSSetState(m_pRasterizerState.Get());
	pDeviceContext_->OMSetBlendState(m_pBlendState.Get(), nullptr, 0xffffffff);
	pDeviceContext_->OMSetDepthStencilState(m_pDepthStencilState.Get(), 0);
	pDeviceContext_->PSSetSamplers(0, m_vecSamplerStates.size(), m_vecSamplerStates.data()->GetAddressOf());
}