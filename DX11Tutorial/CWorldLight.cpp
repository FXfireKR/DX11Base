#include "pch.h"
#include "CWorldLight.h"

WorldLightingParams CWorldLight::Evaluate(const WorldTimeParams& timeParams) const
{
    WorldLightingParams out{};

    // ---------- sun / moon direction ----------
    const float z = cosf(timeParams.sunAngleRad);
    const float y = timeParams.sunHeight;

    XMVECTOR vSun = XMVector3Normalize(XMVectorSet(0.25f, y, z, 0.f));
    XMVECTOR vMoon = XMVectorNegate(vSun);

    XMStoreFloat3(&out.sunDir, vSun);
    XMStoreFloat3(&out.moonDir, vMoon);

    // ---------- intensity ----------
    out.sunIntensity = saturate(timeParams.daylight * 1.15f);

    // ---------- ambient ----------
    const XMFLOAT3 night = { 0.05f, 0.07f, 0.10f };
    const XMFLOAT3 day = { 0.28f, 0.30f, 0.33f };
    out.ambientColor = _LerpColor(night, day, timeParams.daylight);

    // ---------- sky ----------
    const XMFLOAT3 skyNight = { 0.02f, 0.03f, 0.06f };
    const XMFLOAT3 skyDawn = { 0.45f, 0.28f, 0.18f };
    const XMFLOAT3 skyDay = { 0.55f, 0.72f, 0.95f };

    if (timeParams.daylight < 0.35f)
    {
        float t = saturate(timeParams.daylight / 0.35f);
        out.skyColor = _LerpColor(skyNight, skyDawn, t);
    }
    else // (timeParams.daylight >= 0.35f)
    {
        float t = saturate((timeParams.daylight - 0.35f) / 0.65f);
        out.skyColor = _LerpColor(skyDawn, skyDay, t);
    }

    // ---------- sun color ----------
    const XMFLOAT3 warm = { 1.0f, 0.82f, 0.62f };
    const XMFLOAT3 noon = { 1.0f, 0.97f, 0.92f };
    out.sunColor = _LerpColor(warm, noon, timeParams.daylight);

    // ---------- shadow ----------
    out.shadowEnabled = (timeParams.daylight > 0.08f);

    return out;
}

XMFLOAT3 CWorldLight::_LerpColor(const XMFLOAT3& a, const XMFLOAT3& b, float t) const
{
    return {
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t
    };
}
