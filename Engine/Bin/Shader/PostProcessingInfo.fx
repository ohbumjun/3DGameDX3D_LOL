#include "ShaderInfo.fx"

// Final Target �� �������� ���̴�.
Texture2DMS<float4> HDRTex : register(t81);

// �ֵ� ����� ���� ��� 
static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);