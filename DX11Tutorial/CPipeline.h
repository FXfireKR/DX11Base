#pragma once

class CPipeline
{
public:
	CPipeline() = default;
	~CPipeline() = default;

	void Init(uint64_t shaderID_, uint32_t shaderFlags_, uint32_t uInputLayoutID_);

	void Begin(ID3D11DeviceContext* pDeviceContext_);
	void End(ID3D11DeviceContext* pDeviceContext_);


private:
	uint64_t m_uShaderID;
	uint32_t m_uShaderFlags;
	uint32_t m_uInputLayoutID = UINT32_MAX;

	// Render states
	ComPtr<ID3D11RasterizerState> m_pRasterizerState;
	ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;
	ComPtr<ID3D11BlendState> m_pBlendState;
};