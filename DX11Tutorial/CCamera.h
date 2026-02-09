#pragma once
#include "CComponentBase.h"

class CTransform;

enum class CAMERA_PROJECTION_TYPE
{
	PERSPECTIVE,	// 원근 (깊이, 거리에 따른 랜더 차등)
	ORTHOGRAPHIC,	// 직교 (깊이, 거리에 관계 없는 동일 랜더)
};

struct KPerspectiveParams
{
	float fFieldOfView = XM_PI / 4.0f;
	float fAspectRatio = 1440.0f / 1024.0f;
	float fNearZ = 0.1f;
	float fFarZ = 1000.0f;
};

struct KOrthographicParams
{
	float fWidth = 10.0f;
	float fHeight = 10.0f;
	float fNearZ = 0.1f;
	float fFarZ = 1000.0f;
};

class CCamera : public CComponentBase<CCamera, COMPONENT_TYPE::CAMERA>
{
public:
	CCamera();
	virtual ~CCamera();

	void Init() override;
	void LateUpdate() override;

	void UpdateCameraMatrix();

	inline const CAMERA_PROJECTION_TYPE& GetProjectionType() const { return m_eProjectionType; }
	inline void SetProjectionType(const CAMERA_PROJECTION_TYPE& eProjectionType_) { m_eProjectionType = eProjectionType_; }

	inline const XMMATRIX& GetViewMatrix() const { return m_matView; }
	inline const XMMATRIX& GetProjMatrix() const { return m_matProjection; }

	inline const KPerspectiveParams& GetPerspectiveParams() const { return m_kPerspective; }
	inline const KOrthographicParams& GetOrthographicParams() const { return m_kOrthographic; }

private:
	CAMERA_PROJECTION_TYPE m_eProjectionType = CAMERA_PROJECTION_TYPE::PERSPECTIVE;
	
	XMMATRIX m_matView = XMMatrixIdentity();
	XMMATRIX m_matProjection = XMMatrixIdentity();

	KPerspectiveParams m_kPerspective;
	KOrthographicParams m_kOrthographic;
}; 