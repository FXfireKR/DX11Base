#include "pch.h"
#include "CRenderWorld.h"

CRenderWorld::~CRenderWorld()
{
}

void CRenderWorld::Initialize(HWND hWnd_, int iScreenWidth_, int iScreenHeight_)
{
	m_dxAdapter.Initialize(hWnd_);

	m_shaderManager.Initialize(m_dxAdapter.GetDeviceRef());
	m_renderManager.Initialize(MAX_FRAME_WAIT_QUEUE);

	m_pDevice = m_dxAdapter.GetDevice();
	assert(m_pDevice);

	m_pContext = m_dxAdapter.GetContext();
	assert(m_pContext);

	m_pSwapChain = m_dxAdapter.GetSwapChain();
	assert(m_pSwapChain);

	RECT rc;
	GetClientRect(hWnd_, &rc);
	m_uWidth = rc.right - rc.left;
	m_uHeight = rc.bottom - rc.top;

	_CreateRenderTargetView();
	_CreateDepthStencilView();

	ZeroMemory(&m_kViewPort, sizeof(D3D11_VIEWPORT));
	m_kViewPort.TopLeftX = 0.f;
	m_kViewPort.TopLeftY = 0.f;
	m_kViewPort.Width = static_cast<float>(m_uWidth);
	m_kViewPort.Height = static_cast<float>(m_uHeight);
	m_kViewPort.MinDepth = 0.f;
	m_kViewPort.MaxDepth = 1.f;	
}

void CRenderWorld::BeginFrame()
{
	m_pContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

	m_pContext->ClearRenderTargetView(m_pRenderTargetView.Get(), m_fBackColor);
	m_pContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_pContext->RSSetViewports(1, &m_kViewPort);
}

void CRenderWorld::DrawFrame()
{
	m_renderManager.Draw(m_pContext);
}

void CRenderWorld::EndFrame()
{
	m_pSwapChain->Present(m_dxAdapter.IsVerticalSync() ? 1 : 0, 0);
}

void CRenderWorld::BeginBuildFrame()
{
	m_renderManager.BeginFrame();
}

void CRenderWorld::Submit(const RenderItem& renderItem)
{
	m_renderManager.Submit(renderItem);
}

void CRenderWorld::EndBuildFrame()
{
	m_renderManager.EndFrame();
}

void CRenderWorld::_CreateRenderTargetView()
{
	// Backbuffer -> RTV
	m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&m_pBackBuffer));
	m_pDevice->CreateRenderTargetView(m_pBackBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf());
}

void CRenderWorld::_CreateDepthStencilView()
{
	// Create Depth stencil
	D3D11_TEXTURE2D_DESC depthDesc{};
	depthDesc.Width = m_uWidth;
	depthDesc.Height = m_uHeight;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.CPUAccessFlags = 0;

	m_pDevice->CreateTexture2D(&depthDesc, nullptr, m_pDepthBuffer.GetAddressOf());
	m_pDevice->CreateDepthStencilView(m_pDepthBuffer.Get(), nullptr, m_pDepthStencilView.GetAddressOf());
}