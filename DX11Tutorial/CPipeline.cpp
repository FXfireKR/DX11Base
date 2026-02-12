#include "pch.h"
#include "CPipeline.h"

void CPipeline::CreateRaster(ID3D11Device* pDevice)
{
	D3D11_RASTERIZER_DESC rs = {};
	rs.FillMode = D3D11_FILL_SOLID;
	rs.CullMode = D3D11_CULL_BACK;
	rs.FrontCounterClockwise = FALSE;
	pDevice->CreateRasterizerState(&rs, m_pRasterizerState.GetAddressOf());

	D3D11_DEPTH_STENCIL_DESC dsd{};
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS; // 일반적인 Z Test
	dsd.StencilEnable = false; // 스텐실은 필요 없으면 끔
	pDevice->CreateDepthStencilState(&dsd, m_pDepthStencilState.GetAddressOf());

	//D3D11_BLEND_DESC bd{};
	//bd.AlphaToCoverageEnable = false;
	//bd.IndependentBlendEnable = false;

	////auto& rt = desc.RenderTarget[0];
	////rt.BlendEnable = false;
	////rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//pDevice->CreateBlendState(&bd, m_pBlendState.GetAddressOf());
}

void CPipeline::SetShader(CShader* const pShader_)
{
	if (nullptr == pShader_) {
		assert(false && "Shader nullptr");
		return;
	}

	// vertex shader 
	m_pVertexShader = pShader_->GetVertexShader();

	// pixel shader
	m_pPixelShader = pShader_->GetPixelShader();
}

void CPipeline::SetInputLayout(CInputLayer* const pInputLayout_)
{
	if (nullptr == pInputLayout_) {
		assert(false && "InputLayout nullptr");
		return;
	}

	// ia layout
	m_pInputLayout = pInputLayout_->GetInputLayout();
}

void CPipeline::Bind(ID3D11DeviceContext* pDeviceContext_) const
{
	pDeviceContext_->IASetInputLayout(m_pInputLayout);
	pDeviceContext_->VSSetShader(m_pVertexShader, nullptr, 0);
	pDeviceContext_->PSSetShader(m_pPixelShader, nullptr, 0);

	pDeviceContext_->RSSetState(m_pRasterizerState.Get());
	pDeviceContext_->OMSetDepthStencilState(m_pDepthStencilState.Get(), 0);

	float blendFactor[4] = { 1,0,0,1 };
	pDeviceContext_->OMSetBlendState(m_pBlendState.Get(), blendFactor, 0xffffffff);
}

void CPipeline::Begin(ID3D11DeviceContext* pDeviceContext_)
{
}

void CPipeline::End(ID3D11DeviceContext* pDeviceContext_)
{
}
