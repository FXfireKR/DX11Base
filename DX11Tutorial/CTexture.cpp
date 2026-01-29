#include "pch.h"
#include "CTexture.h"

TextureCreateInfo CTexture::_GetCreateInfo(TextureUsage eUsage_)
{
	switch (eUsage_)
	{
		case TextureUsage::StaticColor: 
			return { 
				D3D11_USAGE_DEFAULT, 
				D3D11_BIND_SHADER_RESOURCE, 
				0,
				false,
				true
			};

		case TextureUsage::StaticData:
			return { 
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_SHADER_RESOURCE, 
				0, 
				false,
				false
			};

		case TextureUsage::StaticColorMip:
			return { 
				D3D11_USAGE_DEFAULT, 
				D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, 
				D3D11_RESOURCE_MISC_GENERATE_MIPS, 
				true,
				true 
			};

		case TextureUsage::RenderTarget:
			return {
				D3D11_USAGE_DEFAULT, 
				D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
				0, 
				false,
				false
			};

		case TextureUsage::Depth:
			return { 
				D3D11_USAGE_DEFAULT, 
				D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, 
				0, 
				false,
				false
			};

		case TextureUsage::CubeMapColor:
		case TextureUsage::CubeMapData:
		case TextureUsage::CubeMapRender:
			return { 
				D3D11_USAGE_DEFAULT, 
				D3D11_BIND_SHADER_RESOURCE, 
				D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS, 
				true,
				true
			};
	}
	return {};
}

bool CTexture2D::LoadFromFile(const char* path_, TextureUsage eUsage_)
{
	ComPtr<ID3D11Resource> pResource = nullptr;
	TextureCreateInfo info = _GetCreateInfo(eUsage_);
	wstring path = UTF8ToWstring(path_);
	UINT uLoaderFlags = 0;

	m_kDesc.bSRGB = info.bSRGB;
	m_kDesc.bMipmap = info.bMipmap;

	_CheckTextureSource(path_);

	HRESULT hr = S_OK;
	if (TextureSource::DDS == m_eTextureSource) {

		if (m_kDesc.bSRGB) {
			uLoaderFlags |= DDS_LOADER_FORCE_SRGB;
		}

		hr = CreateDDSTextureFromFileEx(
			DirectX11Com::Device(),
			DirectX11Com::Context(),
			path.c_str(),
			0,
			info.usage,
			info.bindFlags,
			0,
			info.miscFlags,
			static_cast<DDS_LOADER_FLAGS>(uLoaderFlags),
			pResource.GetAddressOf(),
			m_pShaderResourceView.GetAddressOf()
		);
	}
	else {

		if (m_kDesc.bSRGB) {
			uLoaderFlags |= WIC_LOADER_FORCE_SRGB;
		}

		hr = CreateWICTextureFromFileEx(
			DirectX11Com::Device(),
			DirectX11Com::Context(),
			path.c_str(),
			0,
			info.usage,
			info.bindFlags,
			0,
			info.miscFlags,
			static_cast<WIC_LOADER_FLAGS>(uLoaderFlags),
			pResource.GetAddressOf(),
			m_pShaderResourceView.GetAddressOf()
		);
	}	

	if (FAILED(hr)) return false;
	if (nullptr == m_pShaderResourceView) return false;

	if (m_kDesc.bMipmap) {
		DirectX11Com::Context()->GenerateMips(m_pShaderResourceView.Get());
	}

	hr = pResource.As(&m_pTexture);
	if (FAILED(hr)) return false;
	if (nullptr == m_pTexture) return false;

	D3D11_TEXTURE2D_DESC desc;
	m_pTexture->GetDesc(&desc);
	
	m_kDesc.format = desc.Format;
	m_kDesc.width = desc.Width;
	m_kDesc.height = desc.Height;

    return true;
}

void CTexture2D::_CheckTextureSource(const char* path_)
{
	filesystem::path texturePath(path_);
	if (true == texturePath.has_extension()) {
		string ext(texturePath.extension().string());
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		
		if (ext == ".dds") {
			m_eTextureSource = TextureSource::DDS;
		}
		else {
			m_eTextureSource = TextureSource::WIC;
		}
	}
}