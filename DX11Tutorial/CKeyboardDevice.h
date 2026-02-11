#pragma once
#include "IInputDevice.h"

using KeyCode = uint16_t;

class CKeyboardDevice : public IInputDevice
{
public:
	void BeginFrame() override;
	void OnRawInput(const RAWINPUT& raw) override;
	void EndFrame() override;

private:
	static constexpr int KEY_MAX = 256;

	array<ButtonState, KEY_MAX> m_buttons;
};