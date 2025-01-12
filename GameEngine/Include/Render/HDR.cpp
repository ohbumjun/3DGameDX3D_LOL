#include "HDR.h"
#include "../Shader/StructuredBuffer.h"
#include "../Shader/ToneMappingCBuffer.h"
#include "../Shader/FirstHDRDownScaleCBuffer.h"
#include "../Resource/ResourceManager.h"
#include "../Shader/HDRDownScaleFirstPass.h"
#include "../Shader/HDRDownScaleSecondPass.h"
#include "../Engine.h"
#include "../Device.h"

CHDR::CHDR() :
	m_AdaptValue(3.f) // 적응 값은 1 ~ 3초 범위가 가장 적절하다.
{

}

CHDR::~CHDR()
{
	SAFE_DELETE(m_MidDownScaleLumBuffer);
	SAFE_DELETE(m_MeanLumBuffer);
	SAFE_DELETE(m_DownScaleCBuffer);
	SAFE_DELETE(m_ToneMappingCBuffer);
	SAFE_DELETE(m_PrevMeanLumBuffer);
}

bool CHDR::Init()
{
	// First Pass 에서는 전체 화면의 픽셀들을 Group 으로 나눠서 Down Scale 할 때 사용한다.
	// 중간 휘도 (휘도값을 계산할 때 중간값을 저장하는 역할)
	m_MidDownScaleLumBuffer = new CStructuredBuffer;
	
	// Count * sizeof(float) == 4 * Total Back Buffer Pixel  / (16 * 1024);
	// int Count = 1280 * 720 / (16 * 1024);
	Resolution RS = CEngine::GetInst()->GetResolution();
	
	unsigned int TotalPixelCnt = RS.Width * RS.Height;
	// m_FirstDownScaleThreadGrpCnt = (unsigned int)ceil((TotalPixelCnt / (16 * 1024));
	m_FirstDownScaleThreadGrpCnt = TotalPixelCnt / (16 * 1024);
	// m_FirstDownScaleThreadGrpCnt = TotalPixelCnt / (16 * 1024) + 1;

	if (!m_MidDownScaleLumBuffer->Init("MiddleLumBuffer", sizeof(float),
		m_FirstDownScaleThreadGrpCnt, 5, false, (int)Buffer_Shader_Type::Compute))
	{
		SAFE_DELETE(m_MidDownScaleLumBuffer);
		assert(false);
		return false;
	}

	// 부동소수점 형태로 평균 휘도값 저장 
	m_MeanLumBuffer = new CStructuredBuffer;
	
	// Second Pass 에서는 FirstPass 에서 나온 float 값 하나를 넣어준다.
	if (!m_MeanLumBuffer->Init("MeanLumBuffer", sizeof(float), 1, 
		6, false, (int)Buffer_Shader_Type::All))
	{
		SAFE_DELETE(m_MeanLumBuffer);
		assert(false);
		return false;
	}

	// 적응을 위한 이전 프레임 평균 휘도 값 버퍼
	m_PrevMeanLumBuffer = new CStructuredBuffer;

	if (!m_PrevMeanLumBuffer->Init("PrevMeanLumBuffer", sizeof(float), 1,
		7, false, (int)Buffer_Shader_Type::All))
	{
		SAFE_DELETE(m_PrevMeanLumBuffer);
		assert(false);
		return false;
	}

	m_DownScaleFirstPassUpdateShader = (CHDRDownScaleFirstPass*)CResourceManager::GetInst()->FindShader("HDRDownScaleFirstPass");
	m_DownScaleSecondPassUpdateShader = (CHDRDownScaleSecondPass*)CResourceManager::GetInst()->FindShader("HDRDownScaleSecondPass");

	m_DownScaleCBuffer = new CFirstHDRDownScaleCBuffer;

	if (!m_DownScaleCBuffer->Init())
		return false;

	// 변하지 않는 값이다. 처음 한번만 Update 해줄 것이다.
	m_DownScaleCBuffer->UpdateCBuffer();

	m_ToneMappingCBuffer = new CToneMappingCBuffer;

	if (!m_ToneMappingCBuffer->Init())
		return false;

	return true;
}

// First Pass 에서는 3번의 Down Scale 이 일어난다.
// 1) 16 픽셀 그룹은 하나의 픽셀로 줄인다. (쓰레드 그룹 하나에는 1024개의 쓰레드, 각 쓰레드가 16개의 pixeel 을 처리)
// ex) 1280 x 720 의 해상도를 처리하기 위해서는 / (16 * 1024) => 56.25 개의 쓰레드가 존재해야 한다.
// 2) 1024에서 4로 Down Scale
// 3) 4 에서 1 로 Down Scale (4개의 값을 하나의 평균값으로 다운스케일)
void CHDR::RenderFirstDownScale()
{
	// 변하지 않는 값이므로 처음 한번만 Update 한다.
	// m_DownScaleCBuffer->UpdateCBuffer();

	// 쓰기 전용으로 넘겨준다.
	m_MidDownScaleLumBuffer->SetShader();

	m_DownScaleFirstPassUpdateShader->Excute(m_FirstDownScaleThreadGrpCnt, 1, 1);

	// 혹시 모르니 나중에 빼주자 (Second Down Scale 에서)
	m_MidDownScaleLumBuffer->ResetShader();
}

// SecondPass에서도 3번의 DownScale이 일어나는데
// 1. 64에서 16으로 DownScale
// 2. 16에서 4로 DownScale
// 3. 4에서 1로 DownScale
// -> 최종적으로 평균 휘도 값을 구할 수 있다.
void CHDR::RenderSecondDownScale()
{
	// First Pass 에서 해준 상태 
	// m_DownScaleCBuffer->UpdateCBuffer();

	// 쓰기 전용
	m_MeanLumBuffer->SetShader();

	// 중간 휘도 SRV : 읽기 전용으로 넘겨준다.
	m_MidDownScaleLumBuffer->SetShader(75, (int)Buffer_Shader_Type::Compute);

	// 평균 휘도 UAV
	// 2번째에서는 1개의 쓰레드 그룹만 사용하면 된다.
	m_DownScaleSecondPassUpdateShader->Excute(1, 1, 1);

	m_MidDownScaleLumBuffer->ResetShader(75, (int)Buffer_Shader_Type::Compute);

	// 쓰기 전용
	m_MeanLumBuffer->ResetShader();
}

void CHDR::RenderFinalToneMapping()
{
	m_ToneMappingCBuffer->UpdateCBuffer();

	// 최종 SRV : 읽기 전용으로 넘겨준다.
	m_MeanLumBuffer->SetShader(35, (int)Buffer_Shader_Type::All);

	// Null Buffer 출력
	UINT Offset = 0;
	CDevice::GetInst()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	CDevice::GetInst()->GetContext()->IASetVertexBuffers(0, 0, nullptr, nullptr, &Offset);
	CDevice::GetInst()->GetContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	CDevice::GetInst()->GetContext()->Draw(4, 0);

	m_MeanLumBuffer->ResetShader(35, (int)Buffer_Shader_Type::All);

	// 이전 프레임 평균 휘도값 Update (적응 효과를 주기 위함이다.)
	// UpdatePrevMeanLum();
}

void CHDR::UpdatePrevMeanLum()
{
	ID3D11Buffer* TempMeanLumBuffer;
	ID3D11UnorderedAccessView* TempMeanLumUAV;
	ID3D11ShaderResourceView*  TempMeanLumSRV;

	TempMeanLumBuffer = m_MeanLumBuffer->GetBuffer();
	TempMeanLumUAV = m_MeanLumBuffer->GetUAV();
	TempMeanLumSRV = m_MeanLumBuffer->GetSRV();

	m_MeanLumBuffer->SetBuffer(m_PrevMeanLumBuffer->GetBuffer());
	m_MeanLumBuffer->SetUAV(m_PrevMeanLumBuffer->GetUAV());
	m_MeanLumBuffer->SetSRV(m_PrevMeanLumBuffer->GetSRV());

	// 이전 프레임 평균 휘도값에, 현재 프레임 평균 휘도값을 대입해야 한다.
	m_PrevMeanLumBuffer->SetBuffer(TempMeanLumBuffer);
	m_PrevMeanLumBuffer->SetUAV(TempMeanLumUAV);
	m_PrevMeanLumBuffer->SetSRV(TempMeanLumSRV);
}

void CHDR::Update(float DeltaTime)
{
	m_AdaptElapsedTime += DeltaTime;

	// Adaptation 이 3초에 한번씩 실행하게 해주는 코드
	// 시간이 0초부터 3초까지 흐르는 시간을 실행될 시간으로 나누어주면 
	// 선형 보간을 통해 완전 하얀 화면에서 점점 원래의 화면으로 변한다.
	// if (m_AdaptElapsedTime > 3.f)
	if (m_AdaptElapsedTime > m_AdaptValue)
		m_AdaptElapsedTime -= m_AdaptValue;

	// float CalculatedAdaptVal = min(m_AdaptValue < 0.0001f ? m_AdaptValue : m_AdaptElapsedTime / m_AdaptValue, m_AdaptValue - 0.0001f);
	float CalculatedAdaptVal = min(m_AdaptElapsedTime / m_AdaptValue, m_AdaptValue);

	m_DownScaleCBuffer->SetAdaptValue(CalculatedAdaptVal);
}



