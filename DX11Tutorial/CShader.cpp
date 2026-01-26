#include "pch.h"
#include "CShader.h"

CShader::CShader(SHADER_DESC shaderDesc_,  vector<D3D_SHADER_MACRO>&& vecMacros_)
	: m_shaderDesc(std::move(shaderDesc_))
	, m_vecShaderMacro(std::move(vecMacros_)) 
{
	// null-terminated check
	if (true == m_vecShaderMacro.empty() || nullptr != m_vecShaderMacro.back().Name) {
		m_vecShaderMacro.push_back({ nullptr, nullptr });
	}
}

CShader::~CShader() 
{}

void CShader::Apply(ID3D11DeviceContext* pContext_) const
{
	if (nullptr != m_pVertexShader) {
		pContext_->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	}

	if (nullptr != m_pPixelShader) {
		pContext_->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
	}
}

HRESULT CShader::Compile()
{
	// duplicate compile protection
	if (true == m_bIsCompiled) return S_OK;

	HRESULT hr = S_OK;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	if (false == m_shaderDesc.strVertexShaderPath.empty()) 
	{
		wstring wstrPath(m_shaderDesc.strVertexShaderPath.begin(), m_shaderDesc.strVertexShaderPath.end());
		errorBlob.Reset();

		hr = ::D3DCompileFromFile(
			wstrPath.c_str(),
			m_vecShaderMacro.data(),
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			m_shaderDesc.strVertexShaderEntry.c_str(),
			m_shaderDesc.strVertexShaderVersion.c_str(),
#ifdef _DEBUG
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
#else // _DEBUG
			0,
#endif // _DEBUG
			0,
			m_pVertexBlob.GetAddressOf(),
			errorBlob.GetAddressOf()
		);

		if (FAILED(hr)) {
			if (nullptr != errorBlob) {
				cout << (char*)errorBlob->GetBufferPointer() << endl;
			}
			return hr;
		}

		// Create Vertex Blob
		hr = g_Device->CreateVertexShader(
			m_pVertexBlob->GetBufferPointer(),
			m_pVertexBlob->GetBufferSize(),
			nullptr,
			m_pVertexShader.GetAddressOf()
		);

		if (FAILED(hr)) {
			return hr;
		}
	}


	if (false == m_shaderDesc.strPixelShaderPath.empty())
	{
		wstring wstrPath(m_shaderDesc.strPixelShaderPath.begin(), m_shaderDesc.strPixelShaderPath.end());
		errorBlob.Reset();

		hr = ::D3DCompileFromFile(
			wstrPath.c_str(),
			m_vecShaderMacro.data(),
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			m_shaderDesc.strPixelShaderEntry.c_str(),
			m_shaderDesc.strPixelShaderVersion.c_str(),
#ifdef _DEBUG
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
#else // _DEBUG
			0,
#endif // _DEBUG
			0,
			m_pPixelBlob.GetAddressOf(),
			errorBlob.GetAddressOf()
		);

		if (FAILED(hr)) {
			if (nullptr != errorBlob) {
				cout << (char*)errorBlob->GetBufferPointer() << endl;
			}
			return hr;
		}

		// Create Pixel Blob
		hr = g_Device->CreatePixelShader(
			m_pPixelBlob->GetBufferPointer(),
			m_pPixelBlob->GetBufferSize(),
			nullptr,
			m_pPixelShader.GetAddressOf()
		);

		if (FAILED(hr)) {
			return hr;
		}
	}

	m_bIsCompiled = true;
	return hr;
}

void CShader::ReleaseBlobs()
{
	m_pVertexBlob.Reset();
	m_pPixelBlob.Reset();
}