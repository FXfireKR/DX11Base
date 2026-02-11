#pragma once
#include "IGamePadDevice.h"

/*
* https://github.com/nondebug/dualsense
* DualSense 참고
* 
*/

enum class DUALSENSE_BUTTON
{
	CROSS,
	CIRCLE,
	SQUARE,
	TRIANGLE,

	DPAD_UP,
	DPAD_DOWN,
	DPAD_LEFT,
	DPAD_RIGHT,

	L1,
	R1,
	L2,
	R2,

	OPTIONS,
	SHARE,

	LSTICK,
	RSTICK,

	COUNT
};

struct AxisState
{
	float value = 0.0f;
};

struct TriggerState
{
	float value = 0.0f;
};

class CDualSenseDevice : public IGamePadDevice
{
public:
	CDualSenseDevice(GAMEPAD_CONNECT_TYPE eConnectType, GAMEPAD_TYPE eType);

	void BeginFrame() override;
	void OnRawInput(const RAWINPUT& raw) override;
	void EndFrame() override;

private:
	void _ParsingDualSenseUSB(const uint8_t* data);
	void _ParsingDualSenseBT(const uint8_t* data);

private:
	array<ButtonState, (size_t)DUALSENSE_BUTTON::COUNT> m_buttons;

	AxisState m_lx, m_ly, m_rx, m_ry;
	TriggerState m_l2, m_r2;

	uint64_t m_uLastInputTime = 0;
};