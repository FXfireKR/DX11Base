#pragma once

enum class KEY_STATE : uint8_t
{
	NONE,
	DOWN,
	PRESSED,
	UP
};

enum class GAMEPAD_CONNECT_TYPE
{
	USB,
	BLUETOOTH,
};

inline bool GetDeviceName(HANDLE device, wstring& outName)
{
	UINT size = 0;
	GetRawInputDeviceInfo(device, RIDI_DEVICENAME, nullptr, &size);
	if (size == 0) return false;
	outName.resize(size);
	return GetRawInputDeviceInfo(device, RIDI_DEVICENAME, outName.data(), &size) != (UINT)-1;
}

inline bool GetDeviceInfo(HANDLE device, RID_DEVICE_INFO& pDeviceInfo)
{
	pDeviceInfo.cbSize = sizeof(RID_DEVICE_INFO);

	UINT size = pDeviceInfo.cbSize;
	if (GetRawInputDeviceInfo(device, RIDI_DEVICEINFO, &pDeviceInfo, &size) > 0)
		return true;
	return false;
}

inline bool CheckBtDeviceName(HANDLE device)
{
	wstring name = L"";
	if (!GetDeviceName(device, name)) return false;
	return name.find(L"BTHENUM") != std::wstring::npos;
}

inline bool CheckDualSenseBt(const RAWINPUT& raw)
{
	const RAWHID& hid = raw.data.hid;
	if (hid.dwSizeHid < 70) return false;
	const uint8_t* data = reinterpret_cast<const uint8_t*>(hid.bRawData);
	return data[0] == 0x31;
}

inline bool IsBluetooth(const RAWINPUT& raw)
{
	if (CheckBtDeviceName(raw.header.hDevice)) return true;
	if (CheckDualSenseBt(raw)) return true;
	return false;
}

class IInputDevice
{
public:
	virtual ~IInputDevice() = default;
	virtual void BeginFrame() {}
	virtual void EndFrame() {}
	virtual void OnRawInput(const RAWINPUT& raw) {}
};

class IGamePadDevice
{
public:
	virtual ~IGamePadDevice() = default;
	virtual void BeginFrame() PURE;
	virtual void EndFrame() PURE;
	virtual void OnRawInput(const RAWINPUT& raw) PURE;

protected:
	GAMEPAD_CONNECT_TYPE m_eConnectType;
};

class CKeyboardDevice : public IInputDevice
{
public:
	void OnRawInput(const RAWINPUT& raw) override;
};

class CMouseDevice : public IInputDevice
{
public:
	void OnRawInput(const RAWINPUT& raw) override;
};

class CDualSenseDevice : public IGamePadDevice
{
public:
	void BeginFrame() override;
	void EndFrame() override;
	void OnRawInput(const RAWINPUT& raw) override;
};

class CGamePadHub
{
public:
	CGamePadHub() = default;
	~CGamePadHub() = default;

	void BeginFrame();
	void EndFrame();
	void OnRawInput(const RAWINPUT& raw);

private:
	bool _CreatePadDevice(const RAWINPUT& raw);

private:
	unordered_map<HANDLE, unique_ptr<IGamePadDevice>> m_mapDevices;
};

class CRawInputDispatcher
{
public:
	CRawInputDispatcher() = default;
	~CRawInputDispatcher() = default;

	void OnRawInput(const RAWINPUT& raw);

private:
	

private:
	CMouseDevice m_mouse;
	CKeyboardDevice m_keyboard;
	CGamePadHub m_gamePad;
};

class CRawInputManager
{
public:
	CRawInputManager();
	~CRawInputManager();

	void OnRawInput(const RAWINPUT& raw);
	RID_DEVICE_INFO* GetDeviceInfo(HANDLE device);
	void ParseDualSense(const RAWINPUT& raw);

private:

};