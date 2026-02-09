#pragma once
#include "DirectX11Com.h"
#include "CDirectX11Adapter.h"
#include "CRenderManager.h"
#include "CShaderManager.h"
#include "CInputLayerManager.h"
#include "CPipelineManager.h"
#include "CMeshManager.h"

// RenderWorld는 렌더링 결과 하나를 책임지는 부분이다.
// 즉 다수로 구성될 수 있다...

class CRenderWorld
{
public:
	CRenderWorld() = default;
	~CRenderWorld();

	void Initialize(HWND hWnd_, int iScreenWidth_, int iScreenHeight_);

	// Render
	void BeginFrame();
	void DrawFrame();
	void EndFrame();

	// Build render item
	void BeginBuildFrame();
	void Submit(const RenderItem& renderItem);
	void EndBuildFrame();

private:
	void _CreateRenderTargetView();
	void _CreateDepthStencilView();

public:
	inline ID3D11Device* GetDevice() const { return m_dxAdapter.GetDevice(); }
	inline ID3D11DeviceContext* GetContext() const { return m_dxAdapter.GetContext(); }

	inline CShaderManager& GetShaderManager() { return m_shaderManager; }
	inline CInputLayerManager& GetIALayoutManager() { return m_inputLayerManager; }
	inline CPipelineManager& GetPipelineManager() { return m_pipelineManager; }
	inline CMeshManager& GetMeshManager() { return m_meshManager; }

	inline void SetBackColor(float r, float g, float b, float a) {
		m_fBackColor[0] = r;
		m_fBackColor[1] = g;
		m_fBackColor[2] = b;
		m_fBackColor[3] = a;
	}

private:
	CDirectX11Adapter m_dxAdapter;

	CRenderManager m_renderManager;

	CShaderManager m_shaderManager;
	CInputLayerManager m_inputLayerManager;
	CPipelineManager m_pipelineManager;
	CMeshManager m_meshManager;

private:
	ComPtr<ID3D11Texture2D> m_pBackBuffer = nullptr;
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView = nullptr;
	ComPtr<ID3D11Texture2D> m_pDepthBuffer = nullptr;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView = nullptr;

	ID3D11Device* m_pDevice = nullptr; // not-own
	ID3D11DeviceContext* m_pContext = nullptr; // not-own
	IDXGISwapChain* m_pSwapChain = nullptr; // not-own

	D3D11_VIEWPORT m_kViewPort;
	UINT m_uWidth = 0;
	UINT m_uHeight = 0;

	float m_fBackColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

private:
	const size_t MAX_FRAME_WAIT_QUEUE = 3;
};