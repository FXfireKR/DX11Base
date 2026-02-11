#include "pch.h"
#include "CMouseDevice.h"

void CMouseDevice::BeginFrame()
{
	for (auto& iter : m_buttons) {
		iter.up = false;
		iter.down = false;
	}

	m_iDeltaX = 0;
	m_iDeltaY = 0;
	m_fWheel = 0.f;
}

void CMouseDevice::OnRawInput(const RAWINPUT& raw)
{
	const RAWMOUSE& mouse = raw.data.mouse;

	m_iDeltaX += mouse.lLastX;
	m_iDeltaY += mouse.lLastY;

	// Left
	if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
		_Handle(MOUSE_BUTTON::LEFT, true);
	if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
		_Handle(MOUSE_BUTTON::LEFT, false);

	// Right
	if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
		_Handle(MOUSE_BUTTON::RIGHT, true);
	if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
		_Handle(MOUSE_BUTTON::RIGHT, false);

	// Middle
	if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
		_Handle(MOUSE_BUTTON::MIDDLE, true);
	if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
		_Handle(MOUSE_BUTTON::MIDDLE, false);
	
	// wheel
	if (mouse.usButtonFlags & RI_MOUSE_WHEEL)
	{
		 m_fWheel = (float)mouse.usButtonData;
	}
}

void CMouseDevice::EndFrame()
{
}

void CMouseDevice::_Handle(MOUSE_BUTTON eMouseButton, bool bDown)
{
	ButtonState& state = m_buttons[(int)eMouseButton];
	if (bDown && !state.isHeld) state.down = true;
	if (!bDown && state.isHeld) state.up = true;
	state.isHeld = bDown;
}