#pragma once
#include "CComponentBase.h"

class CTransform : public CComponentBase<CTransform, COMPONENT_TYPE::TRANSFORM>
{
public:
	CTransform() = default;
	~CTransform() override = default;

	void Init() override;
	void Update(float fDelta) override;
	void LateUpdate(float fDelta) override;

	void BuildWorldMatrix();

	void AddChild(ObjectID id);
	void SetParent(ObjectID id);

	void SetLocalScale(const XMFLOAT3& fScale);
	void SetLocalRotateEulerDeg(const XMFLOAT3& fRotateDeg);
	void SetLocalRotateEulerRad(const XMFLOAT3& fRotateRad);
	void SetLocalRotateQuat(const XMVECTOR& vRotate);
	void SetLocalTrans(const XMFLOAT3& fTrans);

	inline XMFLOAT3 GetLocalScale() { return m_fScale; }
	inline XMVECTOR GetLocalRotationQuat() { return m_qRotate; }
	inline XMFLOAT3 GetLocalTrans() { return m_fTrans; }

	void RotateLocalQuat(const XMVECTOR& delta);

	void SetWorldScale(const XMFLOAT3& fScale);
	void SetWorldRotationQuat(const XMVECTOR& vRotate);
	void WorldTrans(const XMFLOAT3& fTrans);

	XMFLOAT3 GetWorldScale() const;
	XMVECTOR GetWorldRotationQuat() const;
	XMFLOAT3 GetWorldTrans() const;

	XMFLOAT3 GetRight() const;
	XMFLOAT3 GetUp() const;
	XMFLOAT3 GetLook() const;

	XMFLOAT3 GetRightNorm() const;
	XMFLOAT3 GetUpNorm() const;
	XMFLOAT3 GetLookNorm() const;

	CTransform* GetTransform(ObjectID id);
	const CTransform* GetParent() const;

	inline bool HasParent() const { return m_uParentID != INVALID_OBJECT_ID; }
	inline const XMMATRIX& GetWorldMatrix() const { return m_matWorld; }

private:
	void _MarkLocalDirty();
	void _MarkWorldDirty();

private:
	ObjectID m_uParentID = INVALID_OBJECT_ID;
	vector<ObjectID> m_vecChildren;

	XMFLOAT3 m_fScale = {1.f, 1.f, 1.f};
	XMVECTOR m_qRotate = XMQuaternionIdentity(); // Quaternion
	XMFLOAT3 m_fTrans = {0.f, 0.f, 0.f};
	
	XMMATRIX m_matLocal = XMMatrixIdentity();
	XMMATRIX m_matWorld = XMMatrixIdentity();

	bool m_bLocalDirty = true;
	bool m_bWorldDirty = true;
};