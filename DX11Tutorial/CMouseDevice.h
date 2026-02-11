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

	const bool GetKey(uint16_t vk) const;
	const bool GetKeyDown(uint16_t vk) const;
	const bool GetKeyUp(uint16_t vk) const;

	inline const short& GetWheelCnt() const { return m_sWheel; }
	inline const short GetWheelDir() const { return m_sWheel > 0 ? 1 : -1; }

	inline const int& GetDeltaX() const { return m_iDeltaX; }
	inline const int& GetDeltaY() const { return m_iDeltaY; }

private:
	void _Handle(MOUSE_BUTTON eMouseButton, bool bDown);

private:
	array<ButtonState, (size_t)MOUSE_BUTTON::COUNT> m_buttons;

	int m_iDeltaX = 0;
	int m_iDeltaY = 0;
	short m_sWheel = 0;
};