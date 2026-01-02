#include "pch.h"
#include "Texture.h"

Texture::Texture()
{
}

Texture::~Texture()
{
}

bool Texture::LoadFromFile(const wstring& wstrFileName_)
{
	ComPtr<ID3D11Resource> pResource = nullptr;
	
	if (FAILED(CreateWICTextureFromFile(g_Device.Get(), wstrFileName_.c_str(), pResource.GetAddressOf(), m_pTextureView.GetAddressOf())))
		return false;

	if (FAILED(pResource->QueryInterface(IID_ID3D11Texture2D, (void**)m_pTexture.GetAddressOf())))
		return false;

	D3D11_TEXTURE2D_DESC textureDesc;
	m_pTexture->GetDesc(&textureDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	m_pTextureView->GetDesc(&srvDesc);

	g_DeviceContext->GenerateMips(m_pTextureView.Get());


	//DirectX::TexMetadata md;
	//DirectX::ScratchImage img;
	//HRESULT hr = ::LoadFromWICFile(wstrFileName_.c_str(), WIC_FLAGS_NONE, &md, img);
	//hr = ::CreateShaderResourceView(g_Device.Get(), img.GetImages(), img.GetImageCount(), md, m_pTextureView.GetAddressOf());


	/*hr = ::LoadFromWICFile(L"Golem.png", WIC_FLAGS_NONE, &md, img);
	hr = ::CreateShaderResourceView(_device.Get(), img.GetImages(), img.GetImageCount(), md, _shaderResourveView2.GetAddressOf());*/

	return true;
}