#pragma once
#include "DirectX11Com.h"
#include "CRenderManager.h"
#include "CShaderManager.h"
#include "CInputLayerManager.h"

class CRenderWorld
{
public:
	CRenderWorld() = default;
	~CRenderWorld() = default;

	void Initialize(HWND hWnd_, int iScreenWidth_, int iScreenHeight_);
	void BeginFrame();
	void DrawFrame();
	void EndFrame();

private:
	//void _

public:
	inline CShaderManager& GetShaderManager() { return m_shaderManager; }
	inline CInputLayerManager& GetIALayoutManager() { return m_inputLayerManager; }

	inline ID3D11Device* GetDevice() const { return m_dxManager.GetDevice(); }
	inline ID3D11DeviceContext* GetContext() const { return m_dxManager.GetContext(); }

private:
	CRenderManager m_renderManager;

	CShaderManager m_shaderManager;
	CInputLayerManager m_inputLayerManager;
	DirectX11Com m_dxManager;

private:
	ComPtr<ID3D11Texture2D> m_pBackBuffer = nullptr;
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView = nullptr;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView = nullptr;

	ID3D11Device* m_pDevice = nullptr; // not-own
	ID3D11DeviceContext* m_pContext = nullptr; // not-own

	D3D11_VIEWPORT m_kViewPort;
};