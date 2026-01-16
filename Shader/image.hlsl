
struct VS_INPUT
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer OptionalPosition : register(b0)
{
    float4 oPosition;
}

// IA - VS - RS - PS - OM
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = input.position + oPosition;
    output.uv = input.uv;

    return output;
}

cbuffer OptionalPixel : register(b0)
{
    float4  oColor;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 color = texture0.Sample(sampler0, input.uv);
    saturate(color + oColor);
    return color;
}