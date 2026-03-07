#include "pch.h"
#include "CPlayerController.h"
#include "CCharacterMotor.h"
#include "CBlockInteractor.h"
#include "CInventoryComponent.h"
#include "CObject.h"
#include "CTransform.h"

void CPlayerController::Init()
{
	CInputManager::Get().Mouse().DisalbleMove();
	
	m_fYaw = 0.f;
	m_fPitch = 0.f;
	m_fMouseSensitivity = 0.005f;
	m_fPitchLimitRad = XM_PIDIV2 - 0.05f;
}

void CPlayerController::Start()
{
	m_pOwnTransform = m_pOwner->GetComponent<CTransform>();
	m_pMotor = m_pOwner->GetComponent<CCharacterMotor>();
	m_pBlockInteractor = m_pOwner->GetComponent<CBlockInteractor>();
	m_pInventory = m_pOwner->GetComponent<CInventoryComponent>();
}

void CPlayerController::Update(float fDelta)
{
	if (nullptr == m_pOwnTransform || nullptr == m_pCamTransform) return;

	_UpdateMouseLockToggle();
	_UpdateLook();
	_UpdateMoveIntent();
	_UpdateActionIntent();
	_UpdateHotbarIntent();

	/*if (nullptr == m_pOwnTransform || nullptr == m_pCamTransform) return;

	ImGui::DragFloat("speed", &m_fMoveSpeed, 0.0f, 25.f);
	ImGui::DragFloat("sens", &m_fMouseSensitivity, 0.0f, 0.1f);

	CMouseDevice& mouse = CInputManager::Get().Mouse();
	CKeyboardDevice& key = CInputManager::Get().Keyboard();

	const POINT& delta = mouse.GetDelta();

	m_fYaw += delta.x * m_fMouseSensitivity;
	m_fPitch += delta.y * m_fMouseSensitivity;

	m_fPitch = std::clamp(m_fPitch, -m_fPitchLimitRad, +m_fPitchLimitRad);

	m_pOwnTransform->SetLocalRotateEulerRad({ 0.f, m_fYaw, 0.f });

	m_pCamTransform->SetLocalTrans({ 0.f, 0.5f, 0.f });
	m_pCamTransform->SetLocalRotateEulerRad({ m_fPitch, 0.f, 0.f });

 	float fSpeed = m_fMoveSpeed;

	XMVECTOR foward = XMVectorSet(0, 0, 1, 0);
	XMVECTOR right = XMVectorSet(1, 0, 0, 0);
	XMMATRIX yawRot = XMMatrixRotationY(m_fYaw);

	foward = XMVector3TransformNormal(foward, yawRot);
	right = XMVector3TransformNormal(right, yawRot);

	XMVECTOR moveDir = XMVectorSet(0, 0, 0, 0);
	if (key.GetKey('W')) moveDir += foward;
	if (key.GetKey('A')) moveDir -= right;
	if (key.GetKey('S')) moveDir -= foward;
	if (key.GetKey('D')) moveDir += right;

	if (key.GetKeyUp(VK_TAB)) {
		if (mouse.GetMoveLock())
			mouse.EnalbleMove();
		else
			mouse.DisalbleMove();
	}

	moveDir = XMVector3Normalize(moveDir);

	if (!XMVector3Equal(moveDir, XMVectorZero()))
	{
		XMFLOAT3 pos = m_pOwnTransform->GetWorldTrans();
		XMVECTOR vPos = XMLoadFloat3(&pos);

		vPos += moveDir * (fSpeed * fDelta);

		XMStoreFloat3(&pos, vPos);
		m_pOwnTransform->SetLocalTrans(pos);
	}*/
}

void CPlayerController::SetCameraTransform(CTransform* pTransform)
{ 
	m_pCamTransform = pTransform; 
}

void CPlayerController::_UpdateMouseLockToggle()
{
	CMouseDevice& mouse = CInputManager::Get().Mouse();

	if (CInputManager::Get().Keyboard().GetKeyUp(VK_TAB))
	{
		if (mouse.GetMoveLock())
			mouse.EnalbleMove();
		else
			mouse.DisalbleMove();
	}
}

void CPlayerController::_UpdateLook()
{
	CMouseDevice& mouse = CInputManager::Get().Mouse();
	const POINT& delta = mouse.GetDelta();

	m_fYaw += delta.x * m_fMouseSensitivity;
	m_fPitch += delta.y * m_fMouseSensitivity;

	m_fPitch = std::clamp(m_fPitch, -m_fPitchLimitRad, m_fPitchLimitRad);

	m_pOwnTransform->SetLocalRotateEulerRad({ 0.f, m_fYaw, 0.f });

	m_pCamTransform->SetLocalTrans({ 0.f, 1.5f, 0.f });
	m_pCamTransform->SetLocalRotateEulerRad({ m_fPitch, 0.f, 0.f });

	m_pMotor->SetYaw(m_fYaw);
}

void CPlayerController::_UpdateMoveIntent()
{
	CKeyboardDevice& keyboard = CInputManager::Get().Keyboard();
	XMFLOAT2 moveAxis{ 0.f, 0.f };

	if (keyboard.GetKey('W')) moveAxis.y += 1.f;
	if (keyboard.GetKey('S')) moveAxis.y -= 1.f;
	if (keyboard.GetKey('D')) moveAxis.x += 1.f;
	if (keyboard.GetKey('A')) moveAxis.x -= 1.f;

	m_pMotor->SetMoveInput(moveAxis);

	if (keyboard.GetKeyDown(VK_SPACE))
	{
		m_pMotor->RequestJump();
	}
}

void CPlayerController::_UpdateActionIntent()
{
	CMouseDevice& mouse = CInputManager::Get().Mouse();

	if (mouse.GetKeyDown(VK_LBUTTON))
	{
		m_pBlockInteractor->RequestBreak();
	}

	if (mouse.GetKeyDown(VK_RBUTTON))
	{
		m_pBlockInteractor->RequestPlace();
	}
}

void CPlayerController::_UpdateHotbarIntent()
{
	CKeyboardDevice& keyboard = CInputManager::Get().Keyboard();

	if (keyboard.GetKeyDown('1')) m_pInventory->SetSelectedSlotIndex(0);
	if (keyboard.GetKeyDown('2')) m_pInventory->SetSelectedSlotIndex(1);
	if (keyboard.GetKeyDown('3')) m_pInventory->SetSelectedSlotIndex(2);
	if (keyboard.GetKeyDown('4')) m_pInventory->SetSelectedSlotIndex(3);
	if (keyboard.GetKeyDown('5')) m_pInventory->SetSelectedSlotIndex(4);
	if (keyboard.GetKeyDown('6')) m_pInventory->SetSelectedSlotIndex(5);
	if (keyboard.GetKeyDown('7')) m_pInventory->SetSelectedSlotIndex(6);
	if (keyboard.GetKeyDown('8')) m_pInventory->SetSelectedSlotIndex(7);
	if (keyboard.GetKeyDown('9')) m_pInventory->SetSelectedSlotIndex(8);
}
