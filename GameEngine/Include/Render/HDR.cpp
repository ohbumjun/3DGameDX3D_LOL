#include "HDR.h"
#include "../Shader/StructuredBuffer.h"
#include "../Shader/FirstHDRDownScaleCBuffer.h"
#include "../Resource/ResourceManager.h"
#include "../Shader/HDRDownScaleFirstPass.h"
#include "../Shader/HDRDownScaleSecondPass.h"

CHDR::CHDR()
{
}

CHDR::~CHDR()
{
	SAFE_DELETE(m_MiddleLumBuffer);
	SAFE_DELETE(m_MeanLumBuffer);
	SAFE_DELETE(m_DownScaleCBuffer);
}

bool CHDR::Init()
{
	// First Pass ������ ��ü ȭ���� �ȼ����� Group ���� ������ Down Scale �� �� ����Ѵ�.
	// �߰� �ֵ�
	m_MiddleLumBuffer = new CStructuredBuffer;
	
	// Count * sizeof(float) == 4 * Total Back Buffer Pixel  / (16 * 1024);
	int Count = 1280 * 720 / (16 * 1024);

	if (!m_MiddleLumBuffer->Init("MiddleLumBuffer", sizeof(float), Count, 5, false, (int)Buffer_Shader_Type::Compute))
	{
		SAFE_DELETE(m_MiddleLumBuffer);
		assert(false);
		return false;
	}

	// ��� �ֵ�
	// m_MeanLumBuffer = new CStructuredBuffer;
	// 
	// // Second Pass ������ FirstPass ���� ���� float �� �ϳ��� �־��ش�.
	// if (!m_MeanLumBuffer->Init("MeanLumBuffer", sizeof(float), 1, 36, false, (int)Buffer_Shader_Type::Compute))
	// {
	// 	SAFE_DELETE(m_MeanLumBuffer);
	// 	assert(false);
	// 	return false;
	// }

	m_FirstPassUpdateShader = (CHDRDownScaleFirstPass*)CResourceManager::GetInst()->FindShader("HDRDownScaleFirstPass");
	m_SecondPassUpdateShader = (CHDRDownScaleSecondPass*)CResourceManager::GetInst()->FindShader("HDRDownScaleSecondPass");

	m_DownScaleCBuffer = new CFirstHDRDownScaleCBuffer;

	if (!m_DownScaleCBuffer->Init())
		return false;

	return true;
}

// First Pass ������ 3���� Down Scale �� �Ͼ��.
// 1) 16 �ȼ� �׷��� �ϳ��� �ȼ��� ���δ�.
// 2) 1024���� 4�� Down Scale
// 3) 4 ���� 1 �� Down Scale (4���� ���� �ϳ��� ��հ����� �ٿ����)
void CHDR::RenderFirstDownScale()
{
	m_DownScaleCBuffer->UpdateCBuffer();

	// ���� �������� �Ѱ��ش�.
	m_MiddleLumBuffer->SetShader();

	m_FirstPassUpdateShader->Excute(1, 1, 1);

	m_MiddleLumBuffer->ResetShader();
}

// SecondPass������ 3���� DownScale�� �Ͼ�µ�
// 1. 64���� 16���� DownScale
// 2. 16���� 4�� DownScale
// 3. 4���� 1�� DownScale
// -> ���������� ��� �ֵ� ���� ���� �� �ִ�.
void CHDR::RenderSecondDownScale()
{
	m_DownScaleCBuffer->UpdateCBuffer();

	// �߰� �ֵ� SRV : �б� �������� �Ѱ��ش�.
	m_MiddleLumBuffer->SetShader(15, (int)Buffer_Shader_Type::Compute);

	// ��� �ֵ� UAV
	// m_MeanLumBuffer->SetShader();

	m_SecondPassUpdateShader->Excute(1, 1, 1);

	m_MiddleLumBuffer->ResetShader(15, (int)Buffer_Shader_Type::Compute);

	// m_MeanLumBuffer->ResetShader();
}



