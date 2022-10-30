#include "PostProcessingInfo.fx"

struct VS_TONEMAP_OUTPUT
{
    float4 Pos         : SV_POSITION; // vertex pos
    float4 ProjPos   : POSITION;
};

// �߰� ȸ���� ��� ����, ����� ����� ����, ������ ���� , ���� �㿡 ���� �ٸ��� �����ؾ� �Ѵ�.
cbuffer FinalPassConstants : register(b13)
{
    uint g_MiddleGrey;
    uint g_LumWhiteSqr;
    float2 g_FinalPassEmpty;
};

// Texture2D<float4> HDRTexture : register(t51);
StructuredBuffer<float> AvgLum	: register(t35); // �б� ����

float4 ToneMapping(float3 vHDRColor)
{
    // ���� �ȼ��� ���� �ֵ� ������ ���
    // �߰� ȸ����
    float LScale = dot(vHDRColor, LUM_FACTOR.xyz); // �̰Ŵ� ���� X

    // fLScale *= g_MiddleGrey / AvgLum[0];
    LScale *= g_MiddleGrey / AvgLum[0];

    float FinalScale = (float)0.f;

    // ���� 
    float Numerator = LScale;
    Numerator += (LScale * LScale) / (g_LumWhiteSqr * g_LumWhiteSqr);
    // Numerator += (LScale * LScale) / (g_LumWhiteSqr * g_LumWhiteSqr);
    
    // �и�
    float Denominator = 1.f + LScale;
    FinalScale = Numerator / Denominator;

    // ���� 1 : ��� ���� ���� ����� �о���� ���Ѵ�. (�ذ� => ������� ������ uint �� �߾�� �ߴµ� float ���� �ع��Ⱦ���)
    // ���� 2 : AvgLum[0] �� 0 �� �� ����. 
    // -> 2_1) ��� ���̴����� �߸� ������ְų�  
    // - DownScaleBuffer �� �ѹ��� Update ���༭ �׷��ǰ� ?
    // => �̰� �����ϴ�. ���� ���� 0�� ���´�. (��� ���̴� ����) 
    // 2_2. ���̴� ���ҽ� ��� �о���� ���ϰų�

    // �ֵ� �������� �ȼ� ���� ����
    return float4(vHDRColor * FinalScale, 1.f);
    // return float4(vHDRColor * FinalScale, 1.f);
    // return vHDRColor * 1;
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
    vColor = ToneMapping(vColor.xyz);

    output.Color = vColor;

    // LDR �� ���
    return output;
}
