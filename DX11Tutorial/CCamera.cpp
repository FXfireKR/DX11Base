#include "pch.h"
#include "CCamera.h"
#include "CTransform.h"
#include "CObject.h"
#include "CScene.h"

CCamera::CCamera()
{	// 기본 생성자를 기반으로 상위는 삭제된 기본 생성자가 아닌 변형 생성자 호출
}

CCamera::~CCamera()
{
}

void CCamera::Init()
{
	CTransform* pTransform = m_pOwner->GetComponent<CTransform>();
	pTransform->SetLocalTrans({0.f, 0.f, -5.f});
}

void CCamera::LateUpdate(float fDelta)
{
	auto& input = CInputManager::Get();
	auto& keyboard = input.Keyboard();

	CTransform* pTransform = m_pOwner->GetComponent<CTransform>();
	XMFLOAT3 trans = pTransform->GetLocalTrans();
	if (keyboard.GetKey('W')) {
		trans.z += fDelta * 45.f;
		pTransform->SetLocalTrans(trans);
	}

	if (keyboard.GetKey('S')) {
		trans.z -= fDelta * 45.f;
		pTransform->SetLocalTrans(trans);
	}

	if (keyboard.GetKey('A')) {
		trans.x -= fDelta * 45.f;
		pTransform->SetLocalTrans(trans);
	}

	if (keyboard.GetKey('D')) {
		trans.x += fDelta * 45.f;
		pTransform->SetLocalTrans(trans);
	}

	if (keyboard.GetKey('E')) {
		trans.y -= fDelta * 45.f;
		pTransform->SetLocalTrans(trans);
	}

	if (keyboard.GetKey('Q')) {
		trans.y += fDelta * 45.f;
		pTransform->SetLocalTrans(trans);
	}

	UpdateCameraMatrix();
}

void CCamera::UpdateCameraMatrix()
{
	CTransform* pTransform = m_pOwner->GetComponent<CTransform>();
	if (nullptr == pTransform) return;
	
	const auto& world = pTransform->GetWorldMatrix();

	XMVECTOR eye = world.r[3];
	// look 이 normalize되면 한 좌표에 고정되어 보므로, eye를 더해줘야 움직이는 위치에 맞게 보인다.
	XMVECTOR look = eye + XMVector3Normalize(world.r[2]);
	XMVECTOR up = XMVector3Normalize(world.r[1]);

	// view-matrix
	m_matView = XMMatrixLookAtLH(eye, look, up);

	if (CAMERA_PROJECTION_TYPE::PERSPECTIVE == m_eProjectionType) {
		m_matProjection = XMMatrixPerspectiveFovLH(
			m_kPerspective.fFieldOfView,
			m_kPerspective.fAspectRatio,
			m_kPerspective.fNearZ,
			m_kPerspective.fFarZ
		);
	}
	else {
		m_matProjection = XMMatrixOrthographicLH(
			m_kOrthographic.fWidth,
			m_kOrthographic.fHeight,
			m_kOrthographic.fNearZ,
			m_kOrthographic.fFarZ
		);
	}
}