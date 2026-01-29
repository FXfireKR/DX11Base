#pragma once

enum class TextureUsage
{
	StaticColor,		// 알베도 / 컬러 텍스쳐
	StaticData,			// 노멀 / 러프 / 메탈
	StaticColorMip,		// 컬러 + mip
	RenderTarget,		// 후처리 / GBuffer
	Depth,				// 그림자 / 깊이 pre-pass

	CubeMapColor,			// SkyBox
	CubeMapData,
	CubeMapRender,
};

enum class TextureSource
{
	WIC,
	DDS
};

struct TextureDesc
{
	uint32_t width;
	uint32_t height;
	DXGI_FORMAT format;
	bool bMipmap;
	bool bSRGB;
};

struct TextureCreateInfo
{
	D3D11_USAGE usage;
	UINT bindFlags;
	UINT miscFlags;
	bool bMipmap;
	bool bSRGB;
};

class CTexture
{
protected:
	// block create instance
	CTexture() = default;

public:
	virtual ~CTexture() = default;

	ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView.Get(); }
	ID3D11SamplerState* GetSamplerState() const { return m_pSamplerState.Get(); }

protected:
	TextureCreateInfo _GetCreateInfo(TextureUsage eUsage_);

public:
	inline const TextureDesc& GetDesc() const { return m_kDesc; }
	inline TextureUsage GetUsage() const { return m_eUsage; }

protected:
	ComPtr<ID3D11Texture2D> m_pTexture;
	ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
	ComPtr<ID3D11SamplerState> m_pSamplerState;

	TextureDesc m_kDesc;
	TextureUsage m_eUsage;
};

//class CTextureCube

class CTexture2D : public CTexture 
{
public:
	bool LoadFromFile(const char* path_, TextureUsage eUsage_);

private:
	void _CheckTextureSource(const char* path_);

private:
	TextureSource m_eTextureSource;
};

class CRenderTexture : public CTexture
{
public:
	ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView.Get(); }

private:
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
};

class CDepthTexture : public CTexture
{
public:
	ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView.Get(); }

private:
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
};