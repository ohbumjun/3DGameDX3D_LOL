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

public :
	bool Init();

protected :
	class CStructuredBuffer* m_MiddleLumBuffer;     // �ֵ��� ���� �߰��� �����ϴ� ����
	class CStructuredBuffer* m_MeanLumBuffer;

	class CHDRDownScaleFirstPass*		   m_FirstPassUpdateShader;
	class CHDRDownScaleSecondPass*    m_SecondPassUpdateShader;

	// ��� ���� 2�� 
	class CFirstHDRDownScaleCBuffer* m_DownScaleCBuffer;

	// ���� -> HDR -> (�� ����) LDR
};

