#include "pch.h"
#include "CShader.h"

CShader::CShader(const wstring& wstrVertexShaderPath_, const wstring& wstrPixelShaderPath_,  
	const vector<D3D_SHADER_MACRO>& vecMacros_
)
{
	HRESULT hr = S_OK;

	// Create Vertex Shader
	hr = ::D3DCompileFromFile(
		wstrVertexShaderPath_.c_str(),
		vecMacros_.data(),
		D3D_COMPILE_STANDARD_FILE_INCLUDE, 
		"VS", 
		"vs_5_0", 
#ifdef _DEBUG
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 
#else // _DEBUG
		0,
#endif // _DEBUG
		0, 
		m_pVertexBlob.GetAddressOf(),
		nullptr
	);

	hr = g_Device->CreateVertexShader(
		m_pVertexBlob->GetBufferPointer(),
		m_pVertexBlob->GetBufferSize(),
		nullptr, 
		m_pVertexShader.GetAddressOf()
	);

	
	// Create Pixel Shader
	hr = ::D3DCompileFromFile(
		wstrPixelShaderPath_.c_str(),
		vecMacros_.data(),
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS",
		"ps_5_0",
#ifdef _DEBUG
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
#else // _DEBUG
		0,
#endif // _DEBUG
		0,
		m_pPixelBlob.GetAddressOf(),
		nullptr
	);

	hr = g_Device->CreatePixelShader(
		m_pPixelBlob->GetBufferPointer(),
		m_pPixelBlob->GetBufferSize(),
		nullptr,
		m_pPixelShader.GetAddressOf()
	);
}

CShader::~CShader()
{
}

void CShader::Apply(ID3D11DeviceContext* pContext_) const
{
	pContext_->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	pContext_->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
}
