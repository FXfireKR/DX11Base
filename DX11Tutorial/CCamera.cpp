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
	if (nullptr == pTransform) return;

	pTransform->SetOrigPosition({0.f, 0.f, -5.f});
	pos = { 0.f, 0.f, -5.f };
}

void CCamera::LateUpdate(float fDelta)
{
#ifdef IMGUI_ACTIVATE
	ImGui::Text("X delta : %d", CInputManager::Get().Mouse().GetDeltaX());
	ImGui::Text("Y delta : %d", CInputManager::Get().Mouse().GetDeltaY());
#endif // IMGUI_ACTIVATE

	if (CInputManager::Get().Mouse().GetWheelCnt() != 0)
	{
		pos.z += static_cast<float>(CInputManager::Get().Mouse().GetWheelCnt()) * fDelta * 50.f;
	}

	CTransform* pTransform = m_pOwner->GetComponent<CTransform>();
	pTransform->SetOrigPosition(pos);
}

void CCamera::UpdateCameraMatrix()
{
	CTransform* pTransform = m_pOwner->GetComponent<CTransform>();
	if (nullptr == pTransform) return;
	pTransform->BuildTransform();

	XMFLOAT3 fEye = pTransform->GetOrig().Pos;
	XMFLOAT3 fLook = pTransform->GetLook();
	XMFLOAT3 fUp = pTransform->GetUp();

	XMVECTOR eye = XMLoadFloat3(&fEye);
	XMVECTOR look = XMVectorAdd(eye, XMLoadFloat3(&fLook));
	XMVECTOR up = XMLoadFloat3(&fUp);

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