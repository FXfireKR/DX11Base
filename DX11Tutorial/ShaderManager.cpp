#include "pch.h"
#include "ShaderManager.h"

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

D3D11_INPUT_ELEMENT_DESC imgLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
}

HRESULT ShaderManager::Init()
{
	HRESULT hr = S_OK;

	// Create Vertex Shader
	hr = LoadShaderFromFile(L"..//Shader//image.hlsl", "VS", VERTEX_SHADER_VERSION, m_pVertexShaderBlob);
	hr = g_Device->CreateVertexShader(m_pVertexShaderBlob->GetBufferPointer(), m_pVertexShaderBlob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf());
	assert(SUCCEEDED(hr));

	// Create Layout
	const unsigned long long count = sizeof(imgLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	g_Device->CreateInputLayout(imgLayout, count, m_pVertexShaderBlob->GetBufferPointer(), m_pVertexShaderBlob->GetBufferSize(), m_pInputLayOut.GetAddressOf());

	// Create Pixel Shader
	LoadShaderFromFile(L"..//Shader//image.hlsl", "PS", PIXEL_SHADER_VERSION, m_pPixelShaderBlob);
	hr = g_Device->CreatePixelShader(m_pPixelShaderBlob->GetBufferPointer(), m_pPixelShaderBlob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf());
	assert(SUCCEEDED(hr));

	_CreateSamplerState();
	_CreateRasterizerState();
	_CreateBlendState();

	return hr;
}

HRESULT ShaderManager::LoadShaderFromFile(const wstring& wstrPath_, const string& strName_, const string& strVersion_, ComPtr<ID3DBlob>& pBlob_)
{
	return ::D3DCompileFromFile(wstrPath_.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, strName_.c_str(), strVersion_.c_str()
		, D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, pBlob_.GetAddressOf(), nullptr);
}

void ShaderManager::Render(ID3D11ShaderResourceView* const* pShaderResourceView_ ,ID3D11Buffer* const* pVertexBuffer_, ID3D11Buffer* const* constBuffer_
	, ID3D11Buffer* pIndexBuffer, const UINT& stride_, const UINT& offset_, const UINT& vertcnt_)
{
	// IA
	g_DeviceContext->IASetVertexBuffers(0, 1, pVertexBuffer_, &stride_, &offset_);
	g_DeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_DeviceContext->IASetInputLayout(m_pInputLayOut.Get());
	g_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// VS
	g_DeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	g_DeviceContext->VSSetConstantBuffers(0, 1, constBuffer_);

	// RS
	g_DeviceContext->RSSetState(m_pRasterizerState.Get());

	// PS
	g_DeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
	g_DeviceContext->PSSetShaderResources(0, 1, pShaderResourceView_);
	g_DeviceContext->PSSetConstantBuffers(0, 1, constBuffer_);
	g_DeviceContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());

	// OM
	//g_DeviceContext->OMSetBlendState(m_pBlendState.Get(), nullptr, 0xFFFFFFFF);

	g_DeviceContext->DrawIndexed(6, 0, 0);
}

void ShaderManager::_CreateSamplerState()
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.BorderColor[0] = 1;
	desc.BorderColor[1] = 0;
	desc.BorderColor[2] = 0;
	desc.BorderColor[3] = 1;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxAnisotropy = 16;
	desc.MaxLOD = FLT_MAX;
	desc.MinLOD = FLT_MIN;
	desc.MipLODBias = 0.0f;

	HRESULT hr = g_Device->CreateSamplerState(&desc, m_pSamplerState.GetAddressOf());
}

void ShaderManager::_CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = false;

	HRESULT hr = g_Device->CreateRasterizerState(&desc, m_pRasterizerState.GetAddressOf());
}

void ShaderManager::_CreateBlendState()
{
	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr = g_Device->CreateBlendState(&desc, m_pBlendState.GetAddressOf());
}
