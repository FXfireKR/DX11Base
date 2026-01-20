#pragma once

class CPipeline
{
public:
	CPipeline();
	~CPipeline();

	void Begin(ID3D11DeviceContext* pDeviceContext_);


private:
	ComPtr<ID3D11RasterizerState> m_pRasterizerState = nullptr;
	ComPtr<ID3D11BlendState> m_pBlendState = nullptr;
	ComPtr<ID3D11DepthStencilState> m_pDepthStencilState = nullptr;
	vector<ComPtr<ID3D11SamplerState>> m_vecSamplerStates;
};