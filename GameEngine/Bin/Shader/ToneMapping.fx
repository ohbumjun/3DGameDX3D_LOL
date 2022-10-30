#include "ShaderInfo.fx"

struct VS_TONEMAP_OUTPUT
{
    float4 Pos         : SV_POSITION; // vertex pos
    float4 ProjPos   : POSITION;
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

    output.ProjPos = float4(g_NullPos[VertexID], 0.f, 1.f);
    output.Pos = output.ProjPos;

    return output;
}

// Tone Mapping ������ MiddleGrey�� LunWhiteSqr�� ���� ��⸦ ������ �� �ִ�.
PSOutput_Single ToneMappingPS(VS_TONEMAP_OUTPUT Input)
{
    PSOutput_Single output = (PSOutput_Single)0;

    float2 UV = (float2) 0;
    UV.x = Input.ProjPos.x / Input.ProjPos.w * 0.5f + 0.5f;
    UV.y = Input.ProjPos.y / Input.ProjPos.w * -0.5f + 0.5f;

    int2 TargetPos = (int2) 0;
    TargetPos.x = (int)(UV.x * g_Resolution.x);
    TargetPos.y = (int)(UV.y * g_Resolution.y);

    // ���� ���ø� (HDR ��)
    float4 vColor = HDRTex.Load(TargetPos, 0);

    // �� ����(HDR ���� LDR������ ��ȯ)
    vColor.xyz = ToneMapping(vColor);

    output.Color = vColor;

    // LDR �� ���
    return output;
}
