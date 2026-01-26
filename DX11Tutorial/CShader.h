#pragma once

struct SHADER_DESC
{
	string strVertexShaderPath = "";
	string strVertexShaderVersion = "vs_5_0";
	string strVertexShaderEntry = "VS";
	string strPixelShaderPath = "";
	string strPixelShaderVersion = "ps_5_0";
	string strPixelShaderEntry = "PS";
};

class CShader
{
public:
	CShader(const CShader&) = delete;
	CShader& operator=(const CShader&) = delete;

	CShader(SHADER_DESC shaderDesc_, vector<D3D_SHADER_MACRO>&& vecMacros_);
	~CShader();

	void Apply(ID3D11DeviceContext* pContext_) const;
	HRESULT Compile();
	void ReleaseBlobs();
	
	inline bool GetCompiled() { return m_bIsCompiled; }

	inline ID3D11VertexShader* GetVertexShader() const { return m_pVertexShader.Get(); }
	inline ID3D11PixelShader* GetPixelShader() const { return m_pPixelShader.Get(); }
	inline ID3DBlob* GetVertexBlob() const { return m_pVertexBlob.Get(); }
	inline ID3DBlob* GetPixelBlob() const { return m_pPixelBlob.Get(); }
	
private:
	ComPtr<ID3D11VertexShader> m_pVertexShader = nullptr;
	ComPtr<ID3D11PixelShader> m_pPixelShader = nullptr;
	ComPtr<ID3DBlob> m_pVertexBlob = nullptr;
	ComPtr<ID3DBlob> m_pPixelBlob = nullptr;
	
	vector<ComPtr<ID3D11Buffer>> m_vecVertexConstBuffers;
	vector<ComPtr<ID3D11Buffer>> m_vecPixelConstBuffers;

	SHADER_DESC m_shaderDesc;
	vector<D3D_SHADER_MACRO> m_vecShaderMacro;
	bool m_bIsCompiled = false;
};