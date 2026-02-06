#include "pch.h"
#include "CRenderWorld.h"

CRenderWorld::CRenderWorld()
{
}

CRenderWorld::~CRenderWorld()
{
}

void CRenderWorld::Initialize(HWND hWnd_, int iScreenWidth_, int iScreenHeight_)
{
	assert(SUCCEEDED(m_dxManager.Initialize(hWnd_, iScreenWidth_, iScreenHeight_, false, 1000.0f, 0.3f)));

	m_shaderManager.Initialize(m_dxManager.GetDeviceRef());
	
}