#pragma once

class ICameraRayProvider
{
public:
	virtual ~ICameraRayProvider() = default;
	virtual XMFLOAT3 GetRayOrigin() const PURE;
	virtual XMFLOAT3 GetRayDirection() const PURE;
};