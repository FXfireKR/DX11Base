#pragma once
#include "CRenderManager.h"
#include "CShaderManager.h"
#include "CInputLayerManager.h"
#include "DirectX11Com.h"

class CRenderWorld
{
public:
	CRenderWorld();
	~CRenderWorld();

	void Initialize(HWND hWnd_, int iScreenWidth_, int iScreenHeight_);


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
};