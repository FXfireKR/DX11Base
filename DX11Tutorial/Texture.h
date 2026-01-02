#pragma once

class Texture
{
public:
	Texture();
	~Texture();

	bool LoadFromFile(const wstring& wstrFileName_);

public:
	inline ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() { return m_pTextureView; }

private:
	ComPtr<ID3D11Texture2D> m_pTexture = nullptr;
	ComPtr<ID3D11ShaderResourceView> m_pTextureView = nullptr;
};

