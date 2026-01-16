#pragma once
#include "CComponentBase.h"

struct KTransform
{
	XMFLOAT3 Scale;
	XMFLOAT3 Rotate;
	XMFLOAT3 Pos;
};

using Quaternion = XMFLOAT4;

class CTransform : public CComponentBase<CTransform, COMPONENT_TYPE::TRANSFORM>
{
public:
	CTransform();
	~CTransform();

	void Init() override;
	void Update() override;
	void LateUpdate() override;

	void SetOrig(const KTransform& newTransform_);
	void SetOrigScale(const XMFLOAT3& newScale_);
	void SetOrigRotate(const XMFLOAT3& newRotate_);
	void SetOrigPosition(const XMFLOAT3& newPosition_);

	XMFLOAT3 GetRight() const;
	XMFLOAT3 GetUp() const;
	XMFLOAT3 GetLook() const;

	void BuildTransform();

	void AddChild(ObjectID uChildID_);
	void SetParentID(ObjectID uParentID_);
	const CTransform* GetParent() const;

public:
	// getter
	inline ObjectID GetParentID() { return m_uParentID; }
	inline bool HasParent() { return IsValidObject(m_uParentID); }
	inline bool HasChildren() { return !m_vecChildren.empty(); }
	inline const vector<ObjectID>& GetChildren() const { return m_vecChildren; }

	inline const KTransform& GetLocal() const { return m_kLocalTransform; }
	inline const KTransform& GetOrig() const { return m_kTransform; }
	inline const XMMATRIX& GetWorldMatrix() const { return m_matWorld; }

	// setter
	inline void SetLocal(const KTransform& newTransform_) { m_kLocalTransform = newTransform_; }
	inline void SetLocalScale(const XMFLOAT3& newScale_) { m_kLocalTransform.Scale = newScale_; }
	inline void SetLocalRotate(const XMFLOAT3& newRotate_) { m_kLocalTransform.Rotate = newRotate_; }
	inline void SetLocalPosition(const XMFLOAT3& newPosition_) { m_kLocalTransform.Pos = newPosition_; }
	inline bool GetDirty() { return m_bDirty; }
	inline void SetDirty(bool bDirty_) { m_bDirty = bDirty_; }

private:
	void _QuaternionToEuler(XMFLOAT3* euler_, const XMVECTOR& vQuaternion_) const;

private:
	KTransform m_kLocalTransform = { {1.f, 1.f, 1.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f} };
	KTransform m_kTransform = { {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f} };

	XMMATRIX m_matLocal = XMMatrixIdentity();
	XMMATRIX m_matWorld = XMMatrixIdentity();

private:
	ObjectID m_uParentID = INVALID_OBJECT_ID;
	vector<ObjectID> m_vecChildren;

	// transform-dirty pattern
	bool m_bDirty = false; 
};