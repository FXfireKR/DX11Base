#pragma once
#include "CMouseDevice.h"
#include "CKeyboardDevice.h"
#include "CGamePadHub.h"

#include "singleton.h"

class CInputManager : public singleton<CInputManager>
{
public:
	CInputManager() = default;
	~CInputManager() = default;

	void Initialize(CMouseDevice& mouse, CKeyboardDevice& keyboard, CGamePadHub& gamepad);
	void BeginFrame();
	void EndFrame();

	inline const CMouseDevice& Mouse() const { return *m_pMouse; }
	inline const CKeyboardDevice& Keyboard() const { return *m_pKeyboard; }
	inline const CGamePadHub& GamePad() const { return *m_pGamePad; }

private:
	CMouseDevice* m_pMouse = nullptr;
	CKeyboardDevice* m_pKeyboard = nullptr;
	CGamePadHub* m_pGamePad = nullptr;
};

// TODO: 차후 Action Bind가 필요한경우 활용한 클래스를 만들어내야한다.