
struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORAML;
    float2 uv : TEXCOORD;
    float color : COLOR;
};

cbuffer CBFrame : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
    //float4x4 lightViewMatrix;
};

cbuffer CBObject : register(b1)
{
    float4x4 worldMatrix;
    //float4x4 worldInvMatrix;
};

// IA - VS - RS - PS - OM
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    float4 position = float4(input.position.xyz, 1.0f);

    float4 worldPos = mul(position, worldMatrix);
    float4 viewPos = mul(worldPos, viewMatrix);
    float4 projPos = mul(viewPos, projMatrix);

    output.position = projPos;
    output.uv = input.uv;
    output.normal = input.normal;
    output.color = input.color;

    return output;
}

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.uv;
    float4 color = texture0.Sample(sampler0, uv);
    color *= input.color;
    return color;
    //return float4(input.uv.x, input.uv.y, 0, 1);
    
    //return float4(1,1,1,1);
    //return float4(input.uv.xy, 0, 1); // R=U, G=V, B=0
}