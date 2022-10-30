#include "FirstHDRDownScaleCBuffer.h"
#include "../../Engine.h"
#include "ConstantBuffer.h"

CFirstHDRDownScaleCBuffer::CFirstHDRDownScaleCBuffer() :
	m_BufferData{}
{
}

CFirstHDRDownScaleCBuffer::CFirstHDRDownScaleCBuffer(const CFirstHDRDownScaleCBuffer& Buffer)
{
}

CFirstHDRDownScaleCBuffer::~CFirstHDRDownScaleCBuffer()
{
}

bool CFirstHDRDownScaleCBuffer::Init()
{
	// struct FirstHDRDownScaleCBuffer
	// {
	// 	Vector2 FRes; // ������� ���̿� �ʺ� 4�� ���� ��  (�ʺ� -> ���� ������ )
	// 	int FDomain; // �����ۿ� ���̿� �ʺ� ���� �� 16���� ���� ��
	// 	int FGroupSize; // ������� ���̿� �ʺ� ���� ��, 16���� ���� ���� 1024�� ���� ��
	// };

	Resolution RS = CEngine::GetInst()->GetResolution();

	m_BufferData.FRes           = Vector2((float)RS.Width, (float)RS.Height) / 4.f;
	m_BufferData.FDomain     = (RS.Width / 4.f) * (RS.Height / 4.f); 
	m_BufferData.FGroupSize  = m_BufferData.FDomain * 1024;

	SetConstantBuffer("FirstHDRDownScaleCBuffer");

	return true;
}

void CFirstHDRDownScaleCBuffer::UpdateCBuffer()
{
	m_Buffer->UpdateBuffer(&m_BufferData);
}

CConstantBufferBase* CFirstHDRDownScaleCBuffer::Clone()
{
	return new CFirstHDRDownScaleCBuffer(*this);
}
