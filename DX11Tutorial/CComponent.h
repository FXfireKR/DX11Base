#pragma once

class CObject;

enum class COMPONENT_TYPE : BYTE
{
	TRANSFORM = 0,
	CAMERA,
	MESHRENDER,

	CUSTOM_0,
	// ADD

	END,
};

const int COMPONENT_TYPE_MAX = static_cast<int>(COMPONENT_TYPE::END);

class CComponent
{
protected:
	friend class CObject;

public:
	CComponent();
	virtual ~CComponent();

	virtual void Init() {}
	virtual void FixedUpdate(float fDelta) { UNREFERENCED_PARAMETER(fDelta); }
	virtual void Update(float fDelta) { UNREFERENCED_PARAMETER(fDelta); }
	virtual void LateUpdate(float fDelta) { UNREFERENCED_PARAMETER(fDelta); }
	virtual void Render() {}

	virtual COMPONENT_TYPE GetType() const PURE;

public:
	inline void SetAlive(bool bAlive_) { m_bAlive = bAlive_; }
	inline bool GetAlive() { return m_bAlive; }

	inline void SetEnable(bool bEnable_) { m_bEnable = bEnable_; }
	inline bool GetEnable() { return m_bEnable; }

	inline void SetOwner(CObject* pOwner_) { m_pOwner = pOwner_; }

protected:
	bool m_bAlive = false;
	bool m_bEnable = false;
	CObject* m_pOwner = nullptr;
};