
cbuffer cbPerObject :  register(b0)
{
    float4x4 gWorldViewProj;
};

void vertex_shader(float3 iPosL : POSITION,
    float3 iNormal : NORMAL,
    float4 iColor : COLOR,
    out float4 oPosH : SV_POSITION,
    out float4 oColor: COLOR)
{
    oPosH = mul(float4(iPosL, 1.0f), gWorldViewProj);
    oColor = iColor;
}

float4 fragment_shader(float4 posH : SV_POSITION,
        float4 color : COLOR) : SV_Target
{
    return color;
}

