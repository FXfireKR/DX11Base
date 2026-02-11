#include "pch.h"
#include "CInputManager.h"

void CInputManager::Initialize(CMouseDevice& mouse, CKeyboardDevice& keyboard, CGamePadHub& gamepad)
{
	m_pMouse = &mouse;
	m_pKeyboard = &keyboard;
	m_pGamePad = &gamepad;
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