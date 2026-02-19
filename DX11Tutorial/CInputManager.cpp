#include "pch.h"
#include "CInputManager.h"

void CInputManager::Initialize()
{
}

void CInputManager::BeginFrame()
{
	m_pMouse->BeginFrame();
	m_pKeyboard->BeginFrame();
	m_pGamePad->BeginFrame();
}

void CInputManager::EndFrame()
{
	m_pMouse->EndFrame();
	m_pKeyboard->EndFrame();
	m_pGamePad->EndFrame();
}