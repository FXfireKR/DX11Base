#include "pch.h"
#include "CPlayerController.h"
#include "CObject.h"
#include "CTransform.h"

void CPlayerController::Init()
{
	m_pOwnTransform = m_pOwner->GetComponent<CTransform>();

	CInputManager::Get().Mouse().DisalbleMove();
	m_fMoveSpeed = 15.f;
}

void CPlayerController::Update(float fDelta)
{
	if (nullptr == m_pOwnTransform || nullptr == m_pCamTransform) return;

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
	}
}