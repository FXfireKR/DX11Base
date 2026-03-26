#include "pch.h"
#include "CWorldLight.h"

WorldLightingParams CWorldLight::Evaluate(const WorldTimeParams& timeParams) const
{
    WorldLightingParams out{};

    const float z = cosf(timeParams.sunAngleRad);
    const float y = timeParams.sunHeight;

    XMVECTOR vSun = XMVector3Normalize(XMVectorSet(0.25f, y, z, 0.0f));
    XMVECTOR vMoon = XMVectorNegate(vSun);

    XMStoreFloat3(&out.sunDir, vSun);
    XMStoreFloat3(&out.moonDir, vMoon);

    out.sunIntensity = saturate(timeParams.daylight * 1.15f);

    const XMFLOAT3 ambientNight = { 0.05f, 0.07f, 0.10f };
    const XMFLOAT3 ambientDay = { 0.28f, 0.30f, 0.33f };
    out.ambientColor = _LerpColor(ambientNight, ambientDay, timeParams.daylight);

    const XMFLOAT3 skyNight = { 0.02f, 0.03f, 0.06f };
    const XMFLOAT3 skyDawn = { 0.45f, 0.28f, 0.18f };
    const XMFLOAT3 skyDay = { 0.55f, 0.72f, 0.95f };

    if (timeParams.daylight < 0.35f)
    {
        const float t = saturate(timeParams.daylight / 0.35f);
        out.skyColor = _LerpColor(skyNight, skyDawn, t);
    }
    else // (timeParams.daylight >= 0.35f)
    {
        const float t = saturate((timeParams.daylight - 0.35f) / 0.65f);
        out.skyColor = _LerpColor(skyDawn, skyDay, t);
    }

    const XMFLOAT3 sunWarm = { 1.0f, 0.82f, 0.62f };
    const XMFLOAT3 sunDay = { 1.0f, 0.97f, 0.92f };
    out.sunColor = _LerpColor(sunWarm, sunDay, timeParams.daylight);

    out.shadowEnabled = (timeParams.daylight > 0.08f);

    return out;
}

XMFLOAT3 CWorldLight::_LerpColor(const XMFLOAT3& a, const XMFLOAT3& b, float t) const
{
    
    return XMFLOAT3();
}
