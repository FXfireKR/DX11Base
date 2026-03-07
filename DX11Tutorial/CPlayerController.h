#pragma once
#include "CComponentBase.h"
#include "CBlockInteractor.h"
#include "CInventoryComponent.h"

class CCharacterMotor;
class CBlockInteractor;
class CInventoryComponent;
class CTransform;

class CPlayerController : public CComponentBase<CPlayerController, COMPONENT_TYPE::PLAYERCONTROLLER>
{
public:
	CPlayerController() = default;
	~CPlayerController() override = default;

	void Init() override;
	void Start() override;
	void Update(float fDelta) override;

public:
	void SetCameraTransform(CTransform* pTransform);

private:
	void _UpdateMouseLockToggle();
	void _UpdateLook();
	void _UpdateMoveIntent();
	void _UpdateActionIntent();
	void _UpdateHotbarIntent();

private:
	CTransform* m_pOwnTransform = nullptr;
	CTransform* m_pCamTransform = nullptr;

	CCharacterMotor* m_pMotor = nullptr;
	CBlockInteractor* m_pBlockInteractor = nullptr;
	CInventoryComponent* m_pInventory = nullptr;

	float m_fYaw = 0.f;
	float m_fPitch = 0.f;

	float m_fMouseSensitivity = 0.01f;
	float m_fPitchLimitRad = XM_PIDIV2 - 0.05f;

	/*float m_fYaw = 0.f;
	float m_fPitch = 0.f;

	float m_fMoveSpeed = 0.f;
	float m_fSprintSpeed = 0.f;
	float m_fMouseSensitivity = 0.0025f;
	float m_fPitchLimitRad = XM_PIDIV2 * 0.95f;*/
};