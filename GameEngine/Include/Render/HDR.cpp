#include "HDR.h"
#include "../Shader/StructuredBuffer.h"
#include "../Shader/ToneMappingCBuffer.h"
#include "../Shader/FirstHDRDownScaleCBuffer.h"
#include "../Resource/ResourceManager.h"
#include "../Shader/HDRDownScaleFirstPass.h"
#include "../Shader/HDRDownScaleSecondPass.h"
#include "../Engine.h"

CHDR::CHDR()
{
}

CHDR::~CHDR()
{
	SAFE_DELETE(m_MiddleLumBuffer);
	SAFE_DELETE(m_MeanLumBuffer);
	SAFE_DELETE(m_DownScaleCBuffer);
	SAFE_DELETE(m_ToneMappingCBuffer);
}

bool CHDR::Init()
{
	// First Pass ������ ��ü ȭ���� �ȼ����� Group ���� ������ Down Scale �� �� ����Ѵ�.
	// �߰� �ֵ� (�ֵ����� ����� �� �߰����� �����ϴ� ����)
	m_MiddleLumBuffer = new CStructuredBuffer;
	
	// Count * sizeof(float) == 4 * Total Back Buffer Pixel  / (16 * 1024);
	// int Count = 1280 * 720 / (16 * 1024);
	Resolution RS = CEngine::GetInst()->GetResolution();

	m_FirstDownScaleThreadGrpCnt = ceil(((int)RS.Width * (int)RS.Height) / (16 * 1024));

	if (!m_MiddleLumBuffer->Init("MiddleLumBuffer", sizeof(float), 
		m_FirstDownScaleThreadGrpCnt, 5, false, (int)Buffer_Shader_Type::Compute))
	{
		SAFE_DELETE(m_MiddleLumBuffer);
		assert(false);
		return false;
	}

	// �ε��Ҽ��� ���·� ��� �ֵ��� ���� 
	m_MeanLumBuffer = new CStructuredBuffer;
	
	// Second Pass ������ FirstPass ���� ���� float �� �ϳ��� �־��ش�.
	if (!m_MeanLumBuffer->Init("MeanLumBuffer", sizeof(float), 1, 
		6, false, (int)Buffer_Shader_Type::Compute))
	{
		SAFE_DELETE(m_MeanLumBuffer);
		assert(false);
		return false;
	}

	m_FirstPassUpdateShader = (CHDRDownScaleFirstPass*)CResourceManager::GetInst()->FindShader("HDRDownScaleFirstPass");
	m_SecondPassUpdateShader = (CHDRDownScaleSecondPass*)CResourceManager::GetInst()->FindShader("HDRDownScaleSecondPass");

	m_DownScaleCBuffer = new CFirstHDRDownScaleCBuffer;

	if (!m_DownScaleCBuffer->Init())
		return false;

	m_ToneMappingCBuffer = new CToneMappingCBuffer;

	if (!m_ToneMappingCBuffer->Init())
		return false;

	return true;
}

// First Pass ������ 3���� Down Scale �� �Ͼ��.
// 1) 16 �ȼ� �׷��� �ϳ��� �ȼ��� ���δ�. (������ �׷� �ϳ����� 1024���� ������, �� �����尡 16���� pixeel �� ó��)
// ex) 1280 x 720 �� �ػ󵵸� ó���ϱ� ���ؼ��� / (16 * 1024) => 56.25 ���� �����尡 �����ؾ� �Ѵ�.
// 2) 1024���� 4�� Down Scale
// 3) 4 ���� 1 �� Down Scale (4���� ���� �ϳ��� ��հ����� �ٿ����)
void CHDR::RenderFirstDownScale()
{
	m_DownScaleCBuffer->UpdateCBuffer();

	// ���� �������� �Ѱ��ش�.
	m_MiddleLumBuffer->SetShader();

	// m_FirstPassUpdateShader->Excute(1, 1, 1);
	m_FirstPassUpdateShader->Excute(m_FirstDownScaleThreadGrpCnt, 1, 1);

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

	// ���� ����
	m_MeanLumBuffer->SetShader();

	// �߰� �ֵ� SRV : �б� �������� �Ѱ��ش�.
	m_MiddleLumBuffer->SetShader(15, (int)Buffer_Shader_Type::Compute);

	// ��� �ֵ� UAV
	// m_MeanLumBuffer->SetShader();
	// 2��°������ 1���� ������ �׷츸 ����ϸ� �ȴ�.
	m_SecondPassUpdateShader->Excute(1, 1, 1);

	m_MiddleLumBuffer->ResetShader(15, (int)Buffer_Shader_Type::Compute);

	// ���� ����
	m_MeanLumBuffer->SetShader();

	// m_MeanLumBuffer->ResetShader();
}

void CHDR::FinalToneMapping()
{
	m_ToneMappingCBuffer->UpdateCBuffer();

	// �߰� �ֵ� SRV : �б� �������� �Ѱ��ش�.
	m_MiddleLumBuffer->SetShader(15, (int)Buffer_Shader_Type::Compute);

	m_MiddleLumBuffer->ResetShader(15, (int)Buffer_Shader_Type::Compute);

}



