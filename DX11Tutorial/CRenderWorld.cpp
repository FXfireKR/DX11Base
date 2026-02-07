#include "pch.h"
#include "CRenderWorld.h"

CRenderWorld::~CRenderWorld()
{
}

void CRenderWorld::Initialize(HWND hWnd_, int iScreenWidth_, int iScreenHeight_)
{
	m_dxAdapter.Initialize(hWnd_);
	m_shaderManager.Initialize(m_dxAdapter.GetDeviceRef());
	
	m_pDevice = m_dxAdapter.GetDevice();
	assert(m_pDevice);

	m_pContext = m_dxAdapter.GetContext();
	assert(m_pContext);

	m_pSwapChain = m_dxAdapter.GetSwapChain();
	assert(m_pSwapChain);

	// Backbuffer -> RTV
	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&m_pBackBuffer));
	m_pDevice->CreateRenderTargetView(m_pBackBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf());
	
	RECT rc;
	GetClientRect(hWnd_, &rc);

	// Create Depth stencil
	D3D11_TEXTURE2D_DESC depthDesc{};
	depthDesc.Width = rc.right - rc.left;
	depthDesc.Height = rc.bottom - rc.top;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	m_pDevice->CreateTexture2D(&depthDesc, nullptr, m_pDepthBuffer.GetAddressOf());
	m_pDevice->CreateDepthStencilView(m_pDepthBuffer.Get(), nullptr, m_pDepthStencilView.GetAddressOf());

	ZeroMemory(&m_kViewPort, sizeof(D3D11_VIEWPORT));
	m_kViewPort.TopLeftX = 0.f;
	m_kViewPort.TopLeftY = 0.f;
	m_kViewPort.Width = static_cast<float>(rc.right - rc.left);
	m_kViewPort.Height = static_cast<float>(rc.bottom - rc.top);
	m_kViewPort.MinDepth = 0.f;
	m_kViewPort.MaxDepth = 1.f;


}

void CRenderWorld::BeginFrame()
{
	m_pContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_pContext->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
	m_pContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_pContext->RSSetViewports(1, &m_kViewPort);
}

void CRenderWorld::DrawFrame()
{
}

void CRenderWorld::EndFrame()
{
	m_pSwapChain->Present(m_dxAdapter.IsVerticalSync() ? 1 : 0, 0);
}