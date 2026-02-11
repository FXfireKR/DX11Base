#include "pch.h"
#include "CDualSenseDevice.h"

CDualSenseDevice::CDualSenseDevice(GAMEPAD_CONNECT_TYPE eConnectType, GAMEPAD_TYPE eType)
{
	m_eConnectType = eConnectType;
	m_eGamePadType = eType;
}

void CDualSenseDevice::BeginFrame()
{
	for (auto& iter : m_buttons)
	{
		iter.down = false;
		iter.up = false;
	}
}

void CDualSenseDevice::OnRawInput(const RAWINPUT& raw)
{
	const RAWHID& hid = raw.data.hid;
	if (hid.dwSizeHid < 64) return;

	m_uLastInputTime = GetTickCount64();

	for (int i = 0; i < hid.dwSizeHid; ++i)
	{
		printf("%02X ", hid.bRawData[i]);
	}
	printf("\n");

	if (GAMEPAD_CONNECT_TYPE::BLUETOOTH == m_eConnectType)
	{
		_ParsingDualSenseBT(reinterpret_cast<const uint8_t*>(hid.bRawData));
	}
	else
	{
		_ParsingDualSenseUSB(reinterpret_cast<const uint8_t*>(hid.bRawData));
	}
}

void CDualSenseDevice::EndFrame()
{

}

void CDualSenseDevice::_ParsingDualSenseUSB(const uint8_t* data)
{
	UINT uOff = 3;

	m_lx.value = (data[uOff + 1] - 128) / 128.0f;
	m_ly.value = (data[uOff + 2] - 128) / 128.0f;
	m_rx.value = (data[uOff + 3] - 128) / 128.0f;
	m_ry.value = (data[uOff + 4] - 128) / 128.0f;

	/*m_buttons[(int)DUALSENSE_BUTTON::SQUARE].curr		= (data[uOff+5] & 0x10) != 0;
	m_buttons[(int)DUALSENSE_BUTTON::CROSS].curr		= (data[uOff+5] & 0x20) != 0;
	m_buttons[(int)DUALSENSE_BUTTON::CIRCLE].curr		= (data[uOff+5] & 0x40) != 0;
	m_buttons[(int)DUALSENSE_BUTTON::TRIANGLE].curr		= (data[uOff+5] & 0x80) != 0;

	m_buttons[(int)DUALSENSE_BUTTON::L1].curr			= (data[uOff+6] & 0x01) != 0;
	m_buttons[(int)DUALSENSE_BUTTON::R1].curr			= (data[uOff+6] & 0x02) != 0;

	m_buttons[(int)DUALSENSE_BUTTON::LSTICK].curr		= (data[uOff+7] & 0x04) != 0;
	m_buttons[(int)DUALSENSE_BUTTON::RSTICK].curr		= (data[uOff+7] & 0x08) != 0;*/
}

void CDualSenseDevice::_ParsingDualSenseBT(const uint8_t* data)
{
	UINT uOff = 4;

	m_lx.value = (data[uOff + 1] - 128) / 128.0f;
	m_ly.value = (data[uOff + 2] - 128) / 128.0f;
	m_rx.value = (data[uOff + 3] - 128) / 128.0f;
	m_ry.value = (data[uOff + 4] - 128) / 128.0f;

	//m_buttons[(int)DUALSENSE_BUTTON::SQUARE].curr		= (data[uOff+5] & 0x10) != 0;
	//m_buttons[(int)DUALSENSE_BUTTON::CROSS].curr		= (data[uOff+5] & 0x20) != 0;
	//m_buttons[(int)DUALSENSE_BUTTON::CIRCLE].curr		= (data[uOff+5] & 0x40) != 0;
	//m_buttons[(int)DUALSENSE_BUTTON::TRIANGLE].curr		= (data[uOff+5] & 0x80) != 0;

	//m_buttons[(int)DUALSENSE_BUTTON::L1].curr			= (data[uOff+6] & 0x01) != 0;
	//m_buttons[(int)DUALSENSE_BUTTON::R1].curr			= (data[uOff+6] & 0x02) != 0;

	//m_buttons[(int)DUALSENSE_BUTTON::LSTICK].curr		= (data[uOff+7] & 0x04) != 0;
	//m_buttons[(int)DUALSENSE_BUTTON::RSTICK].curr		= (data[uOff+7] & 0x08) != 0;
}