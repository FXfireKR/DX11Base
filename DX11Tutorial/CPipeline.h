#pragma once
#include "CShader.h"
#include "CInputLayer.h"

class CPipeline
{
public:
	CPipeline() = default;
	~CPipeline() = default;

	void Initialize();

	void CreateRaster(ID3D11Device* pDevice);
	void SetShader(CShader* const pShader_);
	void SetInputLayout(CInputLayer* const pInputLayout_);

	void Bind(ID3D11DeviceContext* pDeviceContext_) const;
	void Begin(ID3D11DeviceContext* pDeviceContext_);
	void End(ID3D11DeviceContext* pDeviceContext_);

	inline const ID3D11RasterizerState* GetRasterizer() const { return m_pRasterizerState.Get(); }
	inline const ID3D11DepthStencilState* GetDepthStencil() const { return m_pDepthStencilState.Get(); }
	inline const ID3D11BlendState* GetBlendState() const { return m_pBlendState.Get(); }

private:
	ID3D11VertexShader* m_pVertexShader = nullptr;
	ID3D11PixelShader* m_pPixelShader = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;

	// Render states
	ComPtr<ID3D11RasterizerState> m_pRasterizerState;
	ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;
	ComPtr<ID3D11BlendState> m_pBlendState;
};