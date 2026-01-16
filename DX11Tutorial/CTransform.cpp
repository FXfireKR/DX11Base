#include "pch.h"
#include "CTransform.h"
#include "CObject.h"
#include "CScene.h"

CTransform::CTransform()
{
}

CTransform::~CTransform()
{
}

void CTransform::Init()
{
}

void CTransform::Update()
{
}

void CTransform::LateUpdate()
{
}

void CTransform::SetOrig(const KTransform& newTransform_)
{
	SetOrigScale(newTransform_.Scale);
	SetOrigRotate(newTransform_.Rotate);
	SetOrigPosition(newTransform_.Pos);
}

void CTransform::SetOrigScale(const XMFLOAT3& newScale_)
{
	XMFLOAT3 newScale = newScale_;
	if (true == HasParent()) 
	{	
		XMFLOAT3 fParentScale = GetParent()->GetOrig().Scale;
		newScale = XMFLOAT3(
			newScale_.x / fParentScale.x,
			newScale_.y / fParentScale.y,
			newScale_.z / fParentScale.z
		);
	}
	SetLocalScale(newScale);
}

void CTransform::SetOrigRotate(const XMFLOAT3& newRotate_)
{
	XMFLOAT3 newRotate = newRotate_;
	if (true == HasParent())
	{
		XMMATRIX matParentWorldInverse = XMMatrixTranspose(GetParent()->GetWorldMatrix());
		XMStoreFloat3(&newRotate, XMVector3TransformNormal(XMLoadFloat3(&newRotate_), matParentWorldInverse));
	}
	SetLocalRotate(newRotate);
}

void CTransform::SetOrigPosition(const XMFLOAT3& newPosition_)
{
	XMFLOAT3 newPosition = newPosition_;
	if (true == HasParent())
	{
		XMMATRIX matParentWorldInverse = XMMatrixTranspose(GetParent()->GetWorldMatrix());
		XMStoreFloat3(&newPosition, XMVector3TransformCoord(XMLoadFloat3(&newPosition_), matParentWorldInverse));
	}
	SetLocalPosition(newPosition);
}

XMFLOAT3 CTransform::GetRight() const
{
	XMFLOAT3 fRight;
	XMStoreFloat3(&fRight, m_matWorld.r[0]);
	return fRight;
}

XMFLOAT3 CTransform::GetUp() const
{
	XMFLOAT3 fUp;
	XMStoreFloat3(&fUp, m_matWorld.r[1]);
	return fUp;
}

XMFLOAT3 CTransform::GetLook() const
{
	XMFLOAT3 fLook;
	XMStoreFloat3(&fLook, m_matWorld.r[2]);
	return fLook;
}

void CTransform::BuildTransform()
{
	if (true == m_bDirty) 
	{	
		// Scale, Rotate, Trans
		XMMATRIX matScale = XMMatrixScaling(m_kLocalTransform.Scale.x, m_kLocalTransform.Scale.y, m_kLocalTransform.Scale.z);
		XMMATRIX matRotate = XMMatrixRotationX(m_kLocalTransform.Rotate.x);
		matRotate *= XMMatrixRotationY(m_kLocalTransform.Rotate.y);
		matRotate *= XMMatrixRotationZ(m_kLocalTransform.Rotate.z);
		XMMATRIX matTranslation = XMMatrixTranslation(m_kLocalTransform.Pos.x, m_kLocalTransform.Pos.y, m_kLocalTransform.Pos.z);

		m_matWorld = m_matLocal = matScale * matRotate * matTranslation;
		if (true == HasParent()) {
			m_matWorld *= GetParent()->GetWorldMatrix();
		}

		XMVECTOR s, r, t;
		if (true == XMMatrixDecompose(&s, &r, &t, m_matWorld)) {
			// Decompose error!..
			return;
		}

		// setting transform
		XMStoreFloat3(&m_kTransform.Scale, s);
		_QuaternionToEuler(&m_kTransform.Rotate, r);
		XMStoreFloat3(&m_kTransform.Pos, t);

		// 이건....일단 두자...
		/*for (const CTransform* pChild : m_vecChild) {
			if (nullptr == pChild) continue;

			
		}*/

		m_bDirty = false;
	}
}

void CTransform::AddChild(ObjectID uChildID_)
{
	if (m_vecChildren.end() == std::find(m_vecChildren.begin(), m_vecChildren.end(), uChildID_)) {
		m_vecChildren.push_back(uChildID_);
	}
}

void CTransform::SetParentID(ObjectID uParentID_)
{
	m_uParentID = uParentID_;
}

const CTransform* CTransform::GetParent() const
{
	if (false == IsValidObject(m_uParentID)) return nullptr;

	const CObject* pParentObject = m_pOwner->GetOwnScene()->FindObject(m_uParentID);
	if (nullptr == pParentObject) return nullptr;
	if (nullptr == pParentObject->GetComponent<CTransform>()) return nullptr;

	return pParentObject->GetComponent<CTransform>();
}

void CTransform::_QuaternionToEuler(XMFLOAT3* euler_, const XMVECTOR& vQuaternion_) const
{
	if (nullptr == euler_) return;
	XMFLOAT4 quaternion_; XMStoreFloat4(&quaternion_, vQuaternion_);

	// roll (x-axis rotation)
	double sinr_cosp = 2 * (quaternion_.w * quaternion_.x + quaternion_.y * quaternion_.z);
	double cosr_cosp = 1 - 2 * (quaternion_.x * quaternion_.x + quaternion_.y * quaternion_.y);
	euler_->x = static_cast<float>(std::atan2(sinr_cosp, cosr_cosp));

	// pitch (y-axis rotation)
	double sinp = std::sqrt(1 + 2 * (quaternion_.w * quaternion_.y - quaternion_.x * quaternion_.z));
	double cosp = std::sqrt(1 - 2 * (quaternion_.w * quaternion_.y - quaternion_.x * quaternion_.z));
	euler_->y = static_cast < float>(2 * std::atan2(sinp, cosp) - 3.14159f / 2);

	// yaw (z-axis rotation)
	double siny_cosp = 2 * (quaternion_.w * quaternion_.z + quaternion_.x * quaternion_.y);
	double cosy_cosp = 1 - 2 * (quaternion_.y * quaternion_.y + quaternion_.z * quaternion_.z);
	euler_->z = static_cast<float>(std::atan2(siny_cosp, cosy_cosp));
}