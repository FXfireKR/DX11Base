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
	m_sWheel = 0;
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
		m_sWheel = static_cast<short>(mouse.usButtonData) / WHEEL_DELTA;
	}
}

void CMouseDevice::EndFrame()
{
}

const bool CMouseDevice::GetKey(uint16_t vk) const
{
	switch (vk)
	{
		case VK_LBUTTON: return m_buttons[(int)MOUSE_BUTTON::LEFT].isHeld;
		case VK_RBUTTON: return m_buttons[(int)MOUSE_BUTTON::RIGHT].isHeld;
		case VK_MBUTTON: return m_buttons[(int)MOUSE_BUTTON::MIDDLE].isHeld;
	}
	return false;
}

const bool CMouseDevice::GetKeyDown(uint16_t vk) const
{
	switch (vk)
	{
		case VK_LBUTTON: return m_buttons[(int)MOUSE_BUTTON::LEFT].down;
		case VK_RBUTTON: return m_buttons[(int)MOUSE_BUTTON::RIGHT].down;
		case VK_MBUTTON: return m_buttons[(int)MOUSE_BUTTON::MIDDLE].down;
	}
	return false;
}

const bool CMouseDevice::GetKeyUp(uint16_t vk) const
{
	switch (vk)
	{
		case VK_LBUTTON: return m_buttons[(int)MOUSE_BUTTON::LEFT].up;
		case VK_RBUTTON: return m_buttons[(int)MOUSE_BUTTON::RIGHT].up;
		case VK_MBUTTON: return m_buttons[(int)MOUSE_BUTTON::MIDDLE].up;
	}
	return false;
}

void CMouseDevice::_Handle(MOUSE_BUTTON eMouseButton, bool bDown)
{
	ButtonState& state = m_buttons[(int)eMouseButton];
	if (bDown && !state.isHeld) state.down = true;
	if (!bDown && state.isHeld) state.up = true;
	state.isHeld = bDown;
}