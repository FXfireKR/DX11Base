#pragma once
#include "CComponentBase.h"

class CTransform;

class CPlayerController : public CComponentBase<CPlayerController, COMPONENT_TYPE::PLAYERCONTROLLER>
{
public:
	CPlayerController() = default;
	~CPlayerController() override = default;

	void Init() override;
	void Update(float fDelta) override;

	inline void SetCameraTransform(CTransform* pTransform) { m_pCamTransform = pTransform; }

private:
	CTransform* m_pOwnTransform = nullptr;
	CTransform* m_pCamTransform = nullptr;

	float m_fYaw = 0.f;
	float m_fPitch = 0.f;

	float m_fMoveSpeed = 0.f;
	float m_fSprintSpeed = 0.f;
	float m_fMouseSensitivity = 0.0025f;
	float m_fPitchLimitRad = XM_PIDIV2 * 0.95f;
};