#pragma once
#include "singleton.h"

//struct DirectXOVariable
//{
//	
//}; 

class DirectX11Com : public singleton<DirectX11Com>
{
public:
	DirectX11Com();
	~DirectX11Com();

	HRESULT Initialize(HWND hWnd_, int iScreenWidth_, int iScreenHeight_
        , bool bFullScreen_, float fScreenDepth_, float fScreenNear_);

	void BeginRender();
	void EndRender();
	void SetBackBufferRenderTarget();
	void ResetViewPort();

public:
	void ImGuiTick();
	void ImGuiRender();

private:
	HRESULT _ImGuiInit(HWND hWnd_);
	void _ImGuiRelease();

public:
	inline ComPtr<ID3D11Device> GetDevice() const { return m_pDevice; }
	inline ComPtr<ID3D11DeviceContext> GetDeviceContext() const { return m_pDeviceContext; }

	inline void GetProjectionMatrix(XMMATRIX& matWorld_) { matWorld_ = m_matProjection; }
	inline void GetWorldMatrix(XMMATRIX& matWorld_) { matWorld_ = m_matWorld; }
	inline void GetOrthoMatrix(XMMATRIX& matWorld_) { matWorld_ = m_matOrtho; }

private:
	bool m_bVerticalSync;
	double m_dVideoCardMemory;
	UINT m_uDXGIDisplayWidth;
	UINT m_uDXGIDisplayHeight;
	DXGI_RATIONAL m_uDXGIDisplayRational;
	
	ComPtr<ID3D11Device> m_pDevice = nullptr;
	ComPtr<ID3D11DeviceContext> m_pDeviceContext = nullptr;
	ComPtr<IDXGISwapChain> m_pSwapChain = nullptr;
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView = nullptr;

    ID3D11Texture2D* m_pDepthStencilBuffer;
    ID3D11DepthStencilState* m_pDepthStencilState;
    ID3D11DepthStencilView* m_pDepthStencilView;
    ID3D11RasterizerState* m_pRasterState;

    D3D11_VIEWPORT m_kViewPort;
    XMMATRIX m_matProjection;
    XMMATRIX m_matWorld;
    XMMATRIX m_matOrtho;    // Orthographic - 직교적인/정렬된
};