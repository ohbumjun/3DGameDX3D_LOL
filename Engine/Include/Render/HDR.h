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
	class CStructuredBuffer* m_MidDownScaleLumBuffer;     // �ֵ��� ���� �߰��� �����ϴ� ����
	class CStructuredBuffer* m_MeanLumBuffer;

	class CHDRDownScaleFirstPass*		   m_DownScaleFirstPassUpdateShader;
	class CHDRDownScaleSecondPass*    m_DownScaleSecondPassUpdateShader;

	// ��� ���� 2�� 
	class CFirstHDRDownScaleCBuffer* m_DownScaleCBuffer;
	class CToneMappingCBuffer*         m_ToneMappingCBuffer;

private :
	unsigned int  m_FirstDownScaleThreadGrpCnt;
};

// ����
// ���� -> HDR -> (�� ����) LDR
// ��, ���̴� �������� HDR �� ����� �ϰ�, ������ �ÿ��� LDR �� ������ ��ȯ�� �ؾ� �Ѵ�.(�����)
// 1) ȭ���� ��� �ֵ��� ���Ѵ�.
// 2) ��� �ֵ��� Ű������ �Ͽ� ǥ�� ������ 1677����(RGB8) ���� �����Ѵ�.




// ID3D11Buffer* m_MidDownScaleBuffer;
// ID3D11UnorderedAccessView* m_MidDownScaleUAV;
// ID3D11ShaderResourceView* m_MidDownScaleSRV;
// 
// ID3D11Buffer* m_FinalLumAvgBuffer;
// ID3D11UnorderedAccessView* m_FinalLumAvgBufferUAV;
// ID3D11ShaderResourceView* m_FinalLumAvgBufferSRV;