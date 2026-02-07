#pragma once
#include "DirectX11Com.h"
#include "CDirectX11Adapter.h"
#include "CRenderManager.h"
#include "CShaderManager.h"
#include "CInputLayerManager.h"

class CRenderWorld
{
public:
	CRenderWorld() = default;
	~CRenderWorld();

	void Initialize(HWND hWnd_, int iScreenWidth_, int iScreenHeight_);
	void BeginFrame();
	void DrawFrame();
	void EndFrame();

public:
	inline CShaderManager& GetShaderManager() { return m_shaderManager; }
	inline CInputLayerManager& GetIALayoutManager() { return m_inputLayerManager; }

	inline ID3D11Device* GetDevice() const { return m_dxAdapter.GetDevice(); }
	inline ID3D11DeviceContext* GetContext() const { return m_dxAdapter.GetContext(); }

private:
	CRenderManager m_renderManager;

	CShaderManager m_shaderManager;
	CInputLayerManager m_inputLayerManager;
	CDirectX11Adapter m_dxAdapter;

private:
	ComPtr<ID3D11Texture2D> m_pBackBuffer = nullptr;
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView = nullptr;
	ComPtr<ID3D11Texture2D> m_pDepthBuffer = nullptr;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView = nullptr;

	ID3D11Device* m_pDevice = nullptr; // not-own
	ID3D11DeviceContext* m_pContext = nullptr; // not-own
	IDXGISwapChain* m_pSwapChain = nullptr; // not-own

	D3D11_VIEWPORT m_kViewPort;
};