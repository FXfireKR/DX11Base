#include "pch.h"
#include "CMaterial.h"

CMaterial::CMaterial()
{
	m_arrayTexture.fill(nullptr);
	m_arraySamplerState.fill(nullptr);
}

void CMaterial::SetTexture(uint32_t slot, ID3D11ShaderResourceView* shaderReousrceView)
{
	m_arrayTexture[slot] = shaderReousrceView;
}

void CMaterial::SetSampler(uint32_t slot, ID3D11SamplerState* sampler)
{
	m_arraySamplerState[slot] = sampler;
}

void CMaterial::Bind(ID3D11DeviceContext* pContext)
{
	pContext->PSSetShaderResources(0, MAX_TEXTURE_SLOT, m_arrayTexture.data());
	pContext->PSSetSamplers(0, MAX_SAMPLER_SLOT, m_arraySamplerState.data());
}