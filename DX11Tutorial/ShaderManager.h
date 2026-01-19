#pragma once

#define VERTEX_SHADER_VERSION "vs_5_0"
#define PIXEL_SHADER_VERSION "ps_5_0"

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	HRESULT Init();
	HRESULT LoadShaderFromFile(const wstring& wstrPath_, const string& strName_, const string& strVersion_, ComPtr<ID3DBlob>& pBlob_);
	void Render(ID3D11ShaderResourceView* const* pShaderResourceView_, ID3D11Buffer*const* pVertexBuffer_, ID3D11Buffer* const* constBuffer_, ID3D11Buffer* pIndexBuffer, const UINT& stride_, const UINT& offset_, const UINT& vertcnt_);

private:
	void _CreateSamplerState();
	void _CreateRasterizerState();
	void _CreateBlendState();


private:
	ComPtr<ID3D11VertexShader> m_pVertexShader = nullptr;
	ComPtr<ID3DBlob> m_pVertexShaderBlob = nullptr;
	ComPtr<ID3D11PixelShader> m_pPixelShader = nullptr;
	ComPtr<ID3DBlob> m_pPixelShaderBlob = nullptr;
	ComPtr<ID3D11InputLayout> m_pInputLayOut = nullptr;
	ComPtr<ID3D11SamplerState> m_pSamplerState = nullptr;
	ComPtr<ID3D11RasterizerState> m_pRasterizerState = nullptr;
	ComPtr<ID3D11BlendState> m_pBlendState = nullptr;
};