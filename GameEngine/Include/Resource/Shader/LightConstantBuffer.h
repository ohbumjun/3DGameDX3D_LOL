#pragma once

#include "ConstantBufferBase.h"

class CLightConstantBuffer :
    public CConstantBufferBase
{
public:
	CLightConstantBuffer();
	CLightConstantBuffer(const CLightConstantBuffer& Buffer);
	virtual ~CLightConstantBuffer();

protected:
	LightCBuffer	m_BufferData;

public:
	virtual bool Init();
	virtual void UpdateCBuffer();
	virtual CLightConstantBuffer* Clone();

public:
	Light_Type GetLightType()	const
	{
		return (Light_Type)m_BufferData.LightType;
	}

	float GetLightDistance()	const
	{
		return m_BufferData.Distance;
	}

	const Vector3& GetLightPos() const
	{
		return m_BufferData.Pos;
	}

	const Vector3& GetLightDir() const
	{
		return m_BufferData.Dir;
	}

	const Vector4& GetLightColor() const
	{
		return m_BufferData.Color;
	}

	float GetLightAngleIn() const
	{
		return m_BufferData.AngleIn;
	}

	float GetLightAngleOut() const
	{
		return m_BufferData.AngleOut;
	}

	float GetLightAtt1() const
	{
		return m_BufferData.Att1;
	}

	float GetLightAtt2() const
	{
		return m_BufferData.Att2;
	}

	float GetLightAtt3() const
	{
		return m_BufferData.Att3;
	}

public:
	void SetColor(const Vector4& Color)
	{
		m_BufferData.Color = Color;
	}

	void SetLightType(Light_Type LightType)
	{
		m_BufferData.LightType = (int)LightType;
	}

	void SetPos(const Vector3& Pos)
	{
		m_BufferData.Pos = Pos;
	}

	void SetDir(const Vector3& Dir)
	{
		m_BufferData.Dir = Dir;
	}

	void SetDistance(float Distance)
	{
		m_BufferData.Distance = Distance;
	}

	void SetAngleIn(float AngleIn)
	{
		m_BufferData.AngleIn = AngleIn;
	}

	void SetAngleOut(float AngleOut)
	{
		m_BufferData.AngleOut = AngleOut;
	}

	void SetAtt1(float Att1)
	{
		m_BufferData.Att1 = Att1;
	}

	void SetAtt2(float Att2)
	{
		m_BufferData.Att2 = Att2;
	}

	void SetAtt3(float Att3)
	{
		m_BufferData.Att3 = Att3;
	}
};

