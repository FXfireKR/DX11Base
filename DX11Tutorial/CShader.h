#pragma once

class CShader
{
public:
	CShader(const wstring& wstrVertexShaderPath_,
		const wstring& wstrPixelShaderPath_,
		const vector<D3D_SHADER_MACRO>& vecMacros_
	);
	~CShader();

	void Apply(ID3D11DeviceContext* pContext_) const;
	
	inline ID3D11VertexShader* GetVertexShader() const { return m_pVertexShader.Get(); }
	inline ID3D11PixelShader* GetPixelShader() const { return m_pPixelShader.Get(); }
	

private:
	ComPtr<ID3D11VertexShader> m_pVertexShader = nullptr;
	ComPtr<ID3D11PixelShader> m_pPixelShader = nullptr;
	ComPtr<ID3DBlob> m_pVertexBlob = nullptr;
	ComPtr<ID3DBlob> m_pPixelBlob = nullptr;
	

	vector<ComPtr<ID3D11Buffer>> m_vecVertexConstBuffers;
	vector<ComPtr<ID3D11Buffer>> m_vecPixelConstBuffers;
};