#pragma once
#include "CMouseDevice.h"
#include "CKeyboardDevice.h"
#include "CGamePadHub.h"

class CInputManager
{
public:
	CInputManager() = default;
	~CInputManager() = default;

	void Initialize(CMouseDevice& mouse, CKeyboardDevice& keyboard, CGamePadHub& gamepad);
	void BeginFrame();
	void EndFrame();

private:
	CMouseDevice* m_pMouse = nullptr;
	CKeyboardDevice* m_pKeyboard = nullptr;
	CGamePadHub* m_pGamePad = nullptr;
};