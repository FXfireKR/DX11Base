#include "pch.h"
#include "CRawInputDispatcher.h"

void CRawInputDispatcher::DispatchRawQueue()
{
	while (!m_queueRawInput.empty()) 
	{
		_OnRawInput(m_queueRawInput.front());
		m_queueRawInput.pop();
	}
}

void CRawInputDispatcher::_OnRawInput(const RAWINPUT& raw)
{
    _UpdateActiveInputDevice(raw);

	switch (raw.header.dwType)
	{
	case RIM_TYPEMOUSE:
		m_mouse.OnRawInput(raw);
		break;

	case RIM_TYPEKEYBOARD:
		m_keyBoard.OnRawInput(raw);
		break;

	case RIM_TYPEHID:
		m_gamePad.OnRawInput(raw);
		break;
	}
}

void CRawInputDispatcher::_UpdateActiveInputDevice(const RAWINPUT& raw)
{
    switch (raw.header.dwType)
    {
        case RIM_TYPEMOUSE:
        {
            const RAWMOUSE& mouse = raw.data.mouse;
            const bool bMeaningfulMouse =
                (mouse.lLastX != 0) ||
                (mouse.lLastY != 0) ||
                (mouse.usButtonFlags != 0);

            if (bMeaningfulMouse)
            {
                m_eActiveInputDevice = EActiveInputDevice::KEYBOARD_MOUSE;
                m_bGamePadMode = false;
            }
        } break;

        case RIM_TYPEKEYBOARD:
        {
            m_eActiveInputDevice = EActiveInputDevice::KEYBOARD_MOUSE;
            m_bGamePadMode = false;
        } break;

        case RIM_TYPEHID:
        {
            m_eActiveInputDevice = EActiveInputDevice::GAMEPAD;
            m_bGamePadMode = true;
        } break;
    }
}

void CRawInputDispatcher::Push(const RAWINPUT& raw)
{
	m_queueRawInput.push(raw);
}