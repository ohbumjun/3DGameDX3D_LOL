#include "ShaderInfo.fx"

struct VS_TONEMAP_OUTPUT
{
    float4 Pos  : SV_Position; // vertex pos
    float2 UV   : TEXCOORD0;
};

// �߰� ȸ���� ��� ����, ����� ����� ����, ������ ���� , ���� �㿡 ���� �ٸ��� �����ؾ� �Ѵ�.
cbuffer FinalPassConstants : register(s7)
{
    float g_MiddleGrey;
    float g_LumWhiteSqr;
    float2 g_FinalPassEmpty;
}

// �ֵ� ����� ���� ��� 
static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);

// Texture2D<float4> HDRTexture : register(t51);
Texture2DMS<float4> HDRTex : register(t21);
StructuredBuffer<float> AvgLum	: register(t15); // �б� ����

float3 ToneMapping(float3 vHDRColor)
{
    // ���� �ȼ��� ���� �ֵ� ������ ���
    float fLScale = dot(vHDRColor, LUM_FACTOR);
    fLScale *= g_MiddleGrey / AvgLum[0];
    fLScale = (fLScale + fLScale * fLScale / g_LumWhiteSqr) / (1.f + fLScale);

    // �ֵ� �������� �ȼ� ���� ����
    return vHDRColor * fLScale;
}


VS_TONEMAP_OUTPUT ToneMappingVS(uint VertexID : SV_VertexID)
{
    VS_TONEMAP_OUTPUT output = (VS_TONEMAP_OUTPUT)0;

    output.Pos = float4(g_NullPos[VertexID], 0.f, 1.f);
    output.UV = g_NullUV[VertexID].xy;

    return output;
}

// Tone Mapping ������ MiddleGrey�� LunWhiteSqr�� ���� ��⸦ ������ �� �ִ�.
PSOutput_Single ToneMappingPS(VS_TONEMAP_OUTPUT Input)
{
    PSOutput_Single output = (PSOutput_Single)0;

    // ���� ���ø� (HDR ��)
    // float3 vColor = HDRTex.Sample(g_BaseSmp, Input.UV.xy).xyz;
    // float3 vColor = HDRTex.Load(g_BaseSmp, Input.UV.xy).xyz;
    float3 vColor = HDRTex.Load(Input.UV.xy, 0).xyz;

    // �� ����(HDR ���� LDR������ ��ȯ)
    vColor = ToneMapping(vColor);

    output.Color = float4(vColor, 1.f);

    // LDR �� ���
    return output;
}
