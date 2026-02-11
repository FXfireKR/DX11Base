#pragma once
#include "IInputDevice.h"

enum class MOUSE_BUTTON
{
	LEFT,
	RIGHT,
	MIDDLE,

	COUNT
};

class CMouseDevice : public IInputDevice
{
public:
	void BeginFrame() override;
	void OnRawInput(const RAWINPUT& raw) override;
	void EndFrame() override;

private:
	void _Handle(MOUSE_BUTTON eMouseButton, bool bDown);

private:
	array<ButtonState, (size_t)MOUSE_BUTTON::COUNT> m_buttons;

	int m_iDeltaX = 0;
	int m_iDeltaY = 0;
	float m_fWheel = 0.f;
};