#pragma once

#include "../GameInfo.h"
#include "../Resource/Texture/RenderTarget.h"


class CHDR
{
	friend class CRenderManager;
	// 2���� ����
	// 2���� ��� ����
	// 2���� ��ǻ�� ���̴�
protected:
	CHDR();
	CHDR(const CHDR& com) = delete;
	virtual ~CHDR();

public :
	bool Init();
	void RenderFirstDownScale();
	void RenderSecondDownScale();
	void FinalToneMapping();
protected :
	// HDR�� �����ϱ� ���ؼ��� 
	// 1) ShaderResourceView�� UnorderedAccessView�� ������ֱ� ���� ����2��
	// 2) Input���� Shader Resource View 2��, Output������ ����ϴ� UnorderedAccessView 2��
	// 3) ToneMapping�� ��� �ֵ� ���� �־��� ShaderResourceView 1���� �ʿ��ϴ�(���� Shader Resource View �� SRV, UnorderedAccessView�� UAV�� �ϰڴ�)
	class CStructuredBuffer* m_MiddleLumBuffer;     // �ֵ��� ���� �߰��� �����ϴ� ����
	class CStructuredBuffer* m_MeanLumBuffer;

	class CHDRDownScaleFirstPass*		   m_FirstPassUpdateShader;
	class CHDRDownScaleSecondPass*    m_SecondPassUpdateShader;

	// ��� ���� 2�� 
	class CFirstHDRDownScaleCBuffer* m_DownScaleCBuffer;
	class CToneMappingCBuffer*         m_ToneMappingCBuffer;

private :
	int m_FirstDownScaleThreadGrpCnt;
};

// ����
// ���� -> HDR -> (�� ����) LDR
// ��, ���̴� �������� HDR �� ����� �ϰ�, ������ �ÿ��� LDR �� ������ ��ȯ�� �ؾ� �Ѵ�.(�����)
// 1) ȭ���� ��� �ֵ��� ���Ѵ�.
// 2) ��� �ֵ��� Ű������ �Ͽ� ǥ�� ������ 1677����(RGB8) ���� �����Ѵ�.