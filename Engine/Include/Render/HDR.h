#pragma once

#include "../GameInfo.h"
#include "../Resource/Texture/RenderTarget.h"

class CHDR
{
	// 2���� ����
	// 2���� ��� ����
	// 2���� ��ǻ�� ���̴�
protected:
	CHDR();
	CHDR(const CHDR& com) = delete;
	virtual ~CHDR();

protected :
	class CStructuredBuffer* m_FirstDownScaleShaderBuffer;
	class CStructuredBuffer* m_SecondDownScaleShaderBuffer;

	CSharedPtr<CRenderTarget>	m_MidLuminanceTarget;
	CSharedPtr<CRenderTarget>	m_AvgLuminanceTarget;
};

