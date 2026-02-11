#pragma once
#include "CComponent.h"

template<typename t, COMPONENT_TYPE type>
class CComponentBase : public CComponent
{
public:
	CComponentBase() {}
	virtual ~CComponentBase() {}

	virtual void Init() {}
	virtual void FixedUpdate() {}
	virtual void Update(float fDelta) { UNREFERENCED_PARAMETER(fDelta); }
	virtual void LateUpdate(float fDelta) { UNREFERENCED_PARAMETER(fDelta); }
	virtual void Render() {}

	inline static constexpr COMPONENT_TYPE GetStaticType() { return type; }
	inline COMPONENT_TYPE GetType() const override { return GetStaticType(); }
};