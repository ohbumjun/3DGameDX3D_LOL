
#include "PostProcessingInfo.fx"

// 1. Lighting ����
// 2. luminance calculation
// 3. tone-mapping

cbuffer FirstHDRDownScaleCBuffer : register(b7)
{
	float2 g_Res;  // ������� ���̿� �ʺ� 4�� ���� ��  (�ʺ� -> ���� ������ )
	uint   g_Domain;  // �����ۿ� ���̿� �ʺ� ���� �� 16���� ���� ��
	uint   g_GroupSize; // ������� ���̿� �ʺ� ���� ��, 16���� ���� ���� 1024�� ���� ��
	
	float  g_Adaptation; // ������
	float g_DownScaleEmpty;
	float2 g_DownScaleEmpty2;
};

// Render Target ������� �� �Ѱ���� �Ѵ�.
// Texture2D HDRTex : register(t21);

// HDR.cpp �󿡼� m_MiddleLumBuffer
RWStructuredBuffer<float>    AverageLumUAV : register(u5);  // �б�, ���� �Ѵ� ����

// 1��° Pass
// �׷� ���� �޸�  (���� �׷� �� �����峢���� ���� �����Ǵ� ���� ==> �������� ~ ��ġ �޸� ���̿� ��ġ�Ͽ� �б� ���� �ӵ� �� �뷮�� �߰� ����)
// �� 1024���� �����尡 ���� �ϰ� �ִ� ����
groupshared float SharedPositions[1024];

// First Pass ������ 3���� Down Scale �� �Ͼ��.
// 1) 16 �ȼ� �׷��� �ϳ��� �ȼ��� ���δ�.
// 2) 1024���� 4�� Down Scale
// 3) 4 ���� 1 �� Down Scale

// �� �����忡 ���� 4x4 �ٿ� �������� �����Ѵ�
float DownScale4x4(uint2 CurPixel, uint groupThreadId)
{
	// groupThreadId : �� ������ �׷� �������� Idx

	float avgLum = 0.f;

	// �ȼ� ���� ����
	if (CurPixel.y < g_Res.y)
	{
		uint2 iFullResPos = uint2(CurPixel * 4);
		float4 vDownScaled = float4(0.f, 0.f, 0.f, 0.f);

		[unroll]
		for (int i = 0; i < 4; ++i)
		{
			[unroll]
			for (int j = 0; j < 4; ++j)
			{
				vDownScaled += HDRTex.Load(iFullResPos, 0, int2(j, i));
				// vDownScaled += HDRTex.Load(iFullResPos + int2(j, i), 0);
			}
		}

		vDownScaled /= 16;

		// �ȼ��� �ֵ� �� ���
		// ù��° DownScale �� ������, �ֵ� ���ڸ� ���� 
		// �ȼ� ������ �ش��ϴ� �ֵ������� ��ȯ�Ѵ�.
		// �� ������ �ſ� �߿��ϴ�. ����� �̹����� �ٸ� ��ó�� �������� ����ϱ� �����̴�.
		// avgLum = dot(vDownScaled, float4(0.299, 0.587, 0.114, 0));
		avgLum = dot(vDownScaled, LUM_FACTOR);

		// ���� �޸𸮿� ��� ���
		SharedPositions[groupThreadId] = avgLum;
	}

	// ����ȭ �� ���� �ܰ�� (������ �׷� ���� ��� �����尡 �ش� �Լ��� ������ ������ �׷� ���� ��� �������� ������ ���ŷ �Ѵ�.)
	GroupMemoryBarrierWithGroupSync();

	return avgLum;
}

// �� �����忡�� ���� �ֵ����� ����ؼ� ��� 1/4�� �ٿ������� ������ �ݺ��Ѵ�.
// ������ ���� ���� 4���� ������ �ٿ�����Ѵ�
// �� �������� (DownScale4to1 ����) 3/4 �� ������� �ٿ���� ������ �������� �ʰ�
// ���� �����尡 �ش� �������� ���� ���Ͽ� �����Ѵ�.
// �� ������ ������ ���� ������ ����Ǳ� ���� �����带 ����ȭ �Ѵ�.
// �ֳ��ϸ� ��� ������ �׷��� 1024���� �ȼ��� �������� ���� ���� �ֱ� �����̴�.
float DownScale1024to4(uint dispachThreadId, uint groupThreadId,
	float avgLum)
{
	[unroll]
	for (uint iGroupSize = 4, iStep1 = 1, iStep2 = 2, iStep3 = 3;
		iGroupSize < 1024;
		iGroupSize *= 4, iStep1 *= 4, iStep2 *= 4, iStep3 *= 4)
	{
		if (groupThreadId % iGroupSize == 0)
		{
			float fStepAvgLum = avgLum;

			// g_Domain = (1280 * 720) / 16 = 57600
			// dispatchThreadID �ִ���� = 1024 * 57 = 58368 (�Ѿ�� �༮�� �������)

			fStepAvgLum += dispachThreadId + iStep1 < g_Domain ?
				SharedPositions[groupThreadId + iStep1] : avgLum;

			fStepAvgLum += dispachThreadId + iStep2 < g_Domain ?
				SharedPositions[groupThreadId + iStep2] : avgLum;

			fStepAvgLum += dispachThreadId + iStep3 < g_Domain ?
				SharedPositions[groupThreadId + iStep3] : avgLum;

			// ��� �� ����
			avgLum = fStepAvgLum;
			SharedPositions[groupThreadId] = fStepAvgLum;
		}

		// ����ȭ �� ��������
		GroupMemoryBarrierWithGroupSync();
	}

	return avgLum;
}

void DownScale4to1(uint dispatchThreadId, uint groupThreadId,
	uint groupId, float avgLum)
{
	if (groupThreadId == 0)
	{
		//  ������ �׷쿡 ���� ��� �ֵ� �� ���
		float fFinalAvgLum = avgLum;

		fFinalAvgLum += dispatchThreadId + 256 < g_Domain ?
			SharedPositions[groupThreadId + 256] : avgLum;

		fFinalAvgLum += dispatchThreadId + 512 < g_Domain ?
			SharedPositions[groupThreadId + 512] : avgLum;

		fFinalAvgLum += dispatchThreadId + 768 < g_Domain ?
			SharedPositions[groupThreadId + 768] : avgLum;

		fFinalAvgLum /= 1024.f;

		// ������ �׷��� ���� ��� �ȼ��� ���� �ٿ� �������� �Ϸ��ϸ�
		// �ش� ������ ����� �̿��Ͽ� 2��° ��� ���̴��� �����Ѵ�.
		// ���� ���� ID UAV�� ��� �� ���� ��������
		AverageLumUAV[groupId] = fFinalAvgLum;
	}
}

// �̷��� ���� ���� ���̴� ��Ʈ�� ����Ʈ�� ���Եȴ� 
[numthreads(1024, 1, 1)]
void DownScaleFirstPass(uint3 groupId : SV_GroupID, // dispatch ȣ���� ��ü ������ �׷� �߿���, ���� �����尡 ���� �׷� 3���� �ĺ��� 
	uint3 dispatchThreadId : SV_DispatchThreadID,      // ��ü dispatch �ȿ����� ���� �������� 3���� �ĺ��� (�������� ���� ID ��� �� �� �ִ�)
	uint3 groupThreadId : SV_GroupThreadID)             // ���� �����尡 ���� ������ �׷� �ȿ����� Idx
{
	uint2 vCurPixel = uint2(dispatchThreadId.x % (uint)g_Res.x, dispatchThreadId.x / (uint)g_Res.x);

	// 16 �ȼ� �׷��� �ϳ��� �ȼ��� �ٿ� ���� �޸𸮿� ����
	float favgLum = DownScale4x4(vCurPixel, groupThreadId.x);

	// 1024���� 4�� �ٿ����
	favgLum = DownScale1024to4(dispatchThreadId.x, groupThreadId.x,
		favgLum);

	// 4���� 1�� �ٿ����
	DownScale4to1(dispatchThreadId.x, groupThreadId.x, groupId.x,
		favgLum);

	// �� ��ǻƮ ���̴��� x �� (������� �� �ȼ� �� / (16 * 1024)) �� ���� ��� ó���� �� �ִ�
}

// SecondPass������ 3���� DownScale�� �Ͼ�µ�
// 1. 64���� 16���� DownScale
// 2. 16���� 4�� DownScale
// 3. 4���� 1�� DownScale

// ���� : HDR �� ��� ������ ���� �������� ó���Ǿ�� �Ѵ�.
#define MAX_GROUPS 64

// 2��° Pass
// ���� �޸� �׷쿡 �߰� �� ����
groupshared float SharedAvgFinal[MAX_GROUPS];

// HDR.cpp �󿡼� m_MeanLumBuffer
RWStructuredBuffer<float>    AverageLumFinalUAV : register(u6);  // �б�, ���� �Ѵ� ����
// RWStructuredBuffer<float>    PrevAverageLumUAV : register(u7);  // �б�, ���� �Ѵ� ����

// HDR.cpp �󿡼� m_MiddleLumBuffer
StructuredBuffer<float>		    AverageValues1DSRV	: register(t75); // �б� ����

// ù ��° ��ǻƮ ���̴��� ������ �Ϸ�Ǹ� ������ ��� ���۸� ����� �ι�° ��ǻƮ ���̴��� �����Ѵ�
// �߰� �� �ֵ� SRV�� ��� �ֵ� UAV ���� ������ ����Ѵ�
// ��, ù��° ���̴����� �Ѿ�� �߰� ���� �ٽ� �ٿ���� �Ͽ� ���� ��� �ֵ����� ���Ѵ�.
// 1���� ������ �׷츸 ����ص� �ȴ�.
[numthreads(MAX_GROUPS, 1, 1)]
void DownScaleSecondPass(uint3 groupId : SV_GroupID,
	uint3 groupThreadId : SV_GroupThreadID,
	uint3 dispatchThreadId : SV_DispatchThreadID)
{
	// ���� �޸𸮿� ID�� ����
	float favgLum = 0.f;

	if (dispatchThreadId.x < g_GroupSize)
	{
		favgLum = AverageValues1DSRV[dispatchThreadId.x];
	}

	SharedAvgFinal[dispatchThreadId.x] = favgLum;

	GroupMemoryBarrierWithGroupSync(); // ����ȭ �� ���� ��������

	// 64���� 16���� �ٿ� ������
	if (dispatchThreadId.x % 4 == 0)
	{
		// �ֵ� �� �ջ�
		float fstepAvgLum = favgLum;

		fstepAvgLum += dispatchThreadId.x + 1 < g_GroupSize ?
			SharedAvgFinal[dispatchThreadId.x + 1] : favgLum;

		fstepAvgLum += dispatchThreadId.x + 2 < g_GroupSize ?
			SharedAvgFinal[dispatchThreadId.x + 2] : favgLum;

		fstepAvgLum += dispatchThreadId.x + 3 < g_GroupSize ?
			SharedAvgFinal[dispatchThreadId.x + 3] : favgLum;

		// ��� �� ����
		favgLum = fstepAvgLum;

		SharedAvgFinal[dispatchThreadId.x] = fstepAvgLum;
	}

	GroupMemoryBarrierWithGroupSync(); // ����ȭ �� ���� ��������

	// 16 ���� 4�� �ٿ� ������
	// 64���� 16���� �ٿ� ������
	if (dispatchThreadId.x % 16 == 0)
	{
		// �ֵ� �� �ջ�
		float fstepAvgLum = favgLum;

		fstepAvgLum += dispatchThreadId.x + 4 < g_GroupSize ?
			SharedAvgFinal[dispatchThreadId.x + 4] : favgLum;

		fstepAvgLum += dispatchThreadId.x + 8 < g_GroupSize ?
			SharedAvgFinal[dispatchThreadId.x + 8] : favgLum;

		fstepAvgLum += dispatchThreadId.x + 12 < g_GroupSize ?
			SharedAvgFinal[dispatchThreadId.x + 12] : favgLum;

		// ��� �� ����
		favgLum = fstepAvgLum;

		SharedAvgFinal[dispatchThreadId.x] = fstepAvgLum;
	}

	// 4���� 1�� �ٿ����
	if (dispatchThreadId.x == 0)
	{
		// �ֵ� �� �ջ�
		float fFinalLumValue = favgLum;

		fFinalLumValue += dispatchThreadId.x + 16 < g_GroupSize ?
			SharedAvgFinal[dispatchThreadId.x + 16] : favgLum;

		fFinalLumValue += dispatchThreadId.x + 32 < g_GroupSize ?
			SharedAvgFinal[dispatchThreadId.x + 32] : favgLum;

		fFinalLumValue += dispatchThreadId.x + 48 < g_GroupSize ?
			SharedAvgFinal[dispatchThreadId.x + 48] : favgLum;

		fFinalLumValue /= 64.f;

		// ���� �ֵ� ����
		// HDR �������� ���, ī�޶��� �������̳� ���� ��ȭ�� ���� HDR �̹����� ������ �ٲ��.
		// ���� ��� �ֵ����� �ſ� �Ҿ����ϴ�
		// �ش� �Ҿ������� ���� LDR �̹����� ��Ⱑ ���Կ� ���� �� ���� ������ �ε巯���� �ٲ��.
		// ���� ������ ��ο� ������ ������ ��, �츮 ���� �� ��⿡ �����ϱ� ����
		// ��� ������ �ð��� �ʿ�� �Ѵ�. �̷��� ������ �����Ѵ�.

		// ���� �����Ӱ� ���� �������� ��� �ֵ��� Adaptation ����� ������ ���� ���� ó���Ѵ�.
		// float AdaptedAverageLum = lerp(PrevAverageLumUAV[0], fFinalLumValue, g_Adaptation);
		// AverageLumFinalUAV[0] = max(AdaptedAverageLum, 0.001f);
		AverageLumFinalUAV[0] = max(fFinalLumValue, 0.0001f); 

		// AverageLumFinalUAV[0] = fFinalLumValue;

	}
}

// Tone Mapping
// - LDR ���� ����ϴ� LDR ������ Ÿ���� �����ؾ� �Ѵ�.