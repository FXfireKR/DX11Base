#include "pch.h"
#include "CRawInputDispatcher.h"

void CRawInputDispatcher::DispatchRawQueue()
{
	while (!m_queueRawInput.empty())
	{
		const vector<BYTE>& packet = m_queueRawInput.front();

		if (packet.size() >= sizeof(RAWINPUTHEADER))
		{
			const RAWINPUT* raw = reinterpret_cast<const RAWINPUT*>(packet.data());
			if (raw->header.dwSize >= sizeof(RAWINPUTHEADER) && raw->header.dwSize <= packet.size())
				_OnRawInput(*raw);
		}

		m_queueRawInput.pop();
	}
}

void CRawInputDispatcher::_OnRawInput(const RAWINPUT& raw)
{
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

void CRawInputDispatcher::Push(vector<BYTE>&& rawPacket)
{
	m_queueRawInput.push(std::move(rawPacket));
}