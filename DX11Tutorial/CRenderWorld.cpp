#include "pch.h"
#include "CRenderWorld.h"

void CRenderWorld::Initialize(HWND hWnd_, int iScreenWidth_, int iScreenHeight_)
{
	assert(SUCCEEDED(m_dxManager.Initialize(hWnd_, iScreenWidth_, iScreenHeight_, false, 1000.0f, 0.3f)));

	m_shaderManager.Initialize(m_dxManager.GetDeviceRef());
	
	m_pDevice = m_dxManager.GetDevice();
	assert(m_pDevice);

	m_pContext = m_dxManager.GetContext();
	assert(m_pContext);
}

void CRenderWorld::BeginFrame()
{
	m_pContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_pContext->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
	m_pContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void CRenderWorld::DrawFrame()
{
}

void CRenderWorld::EndFrame()
{
	m_dxManager.GetSwapChain()->Present(static_cast<int>(m_dxManager.GetVerticalSync()), 0);
}