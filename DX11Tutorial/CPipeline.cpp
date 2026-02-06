#include "pch.h"
#include "CPipeline.h"

void CPipeline::Initialize(CShader* const pShader_, CInputLayer* const pInputLayer_)
{
	if (nullptr == pShader_ || nullptr == pInputLayer_) {
		assert(false && "Shader or InputLayout nullptr");
		return;
	}

	// vertex shader 
	m_pVertexShader = pShader_->GetVertexShader();

	// pixel shader
	m_pPixelShader = pShader_->GetPixelShader();

	// ia layout
	m_pInputLayout = pInputLayer_->GetInputLayout();
}

void CPipeline::Bind(ID3D11DeviceContext* pDeviceContext_)
{
	assert(pDeviceContext_);

	pDeviceContext_->IASetInputLayout(m_pInputLayout);
	pDeviceContext_->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext_->PSSetShader(m_pPixelShader, nullptr, 0);

	pDeviceContext_->RSSetState(m_pRasterizerState.Get());
	pDeviceContext_->OMSetDepthStencilState(m_pDepthStencilState.Get(), 0);
	pDeviceContext_->OMSetBlendState(m_pBlendState.Get(), nullptr, 0xffffffff);
}

void CPipeline::Begin(ID3D11DeviceContext* pDeviceContext_)
{
}

void CPipeline::End(ID3D11DeviceContext* pDeviceContext_)
{
}
