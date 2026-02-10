#include "pch.h"
#include "CRawInputManager.h"

void CKeyboardDevice::OnRawInput(const RAWINPUT& raw)
{
}

void CMouseDevice::OnRawInput(const RAWINPUT& raw)
{
}

void CDualSenseDevice::BeginFrame()
{
}

void CDualSenseDevice::EndFrame()
{
}

void CDualSenseDevice::OnRawInput(const RAWINPUT& raw)
{
}

void CGamePadHub::BeginFrame()
{

}

void CGamePadHub::EndFrame()
{

}

void CGamePadHub::OnRawInput(const RAWINPUT& raw)
{
	HANDLE hDevice = raw.header.hDevice;
	if (m_mapDevices.find(hDevice) == m_mapDevices.end()) {
		if (false == _CreatePadDevice(raw)) return;
	}
	m_mapDevices[hDevice]->OnRawInput(raw);
}

bool CGamePadHub::_CreatePadDevice(const RAWINPUT& raw)
{
	RID_DEVICE_INFO newDeviceInfo;
	if (false == GetDeviceInfo(raw.header.hDevice, newDeviceInfo)) return false;
	if (newDeviceInfo.hid.dwVendorId != 0x054C) return false;
	if (newDeviceInfo.hid.dwProductId == 0x0CE6)
	{
		if (IsBluetooth(raw))
		{

		}
	}
	
}

void CRawInputDispatcher::OnRawInput(const RAWINPUT& raw)
{
	switch (raw.header.dwType)
	{
		case RIM_TYPEMOUSE:
			m_mouse.OnRawInput(raw);
			break;

		case RIM_TYPEKEYBOARD:
			m_keyboard.OnRawInput(raw);
			break;

		case RIM_TYPEHID:
			m_gamePad.OnRawInput(raw);
			break;
	}	
}

CRawInputManager::CRawInputManager()
{
}

CRawInputManager::~CRawInputManager()
{
}

void CRawInputManager::OnRawInput(const RAWINPUT& raw)
{
	if (raw.header.dwType != RIM_TYPEHID) return;

	const RID_DEVICE_INFO* info = GetDeviceInfo(raw.header.hDevice);

	//if (raw.header.dwType == RIM_TYPEKEYBOARD)
	//{
	//	
	//}

	if (info->hid.dwVendorId == 0x054C && info->hid.dwProductId == 0x0CE6)
	{
		ParseDualSense(raw);
	}
	
}

RID_DEVICE_INFO* CRawInputManager::GetDeviceInfo(HANDLE device)
{
	static RID_DEVICE_INFO info;
	info.cbSize = sizeof(info);

	UINT size = info.cbSize;
	if (GetRawInputDeviceInfo(device, RIDI_DEVICEINFO, &info, &size) > 0)
		return &info;
	return nullptr;
}

void CRawInputManager::ParseDualSense(const RAWINPUT& raw)
{
	const RAWHID& hid = raw.data.hid;
	const uint8_t* data = reinterpret_cast<const uint8_t*>(hid.bRawData);

	if (hid.dwSizeHid < 64) return;

	//
}