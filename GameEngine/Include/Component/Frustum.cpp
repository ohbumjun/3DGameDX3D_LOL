
#include "Frustum.h"

CFrustum::CFrustum()
{
	m_Pos[0] = Vector3(-1.f, 1.f, 0.f);
	m_Pos[1] = Vector3(1.f, 1.f, 0.f);
	m_Pos[2] = Vector3(-1.f, -1.f, 0.f);
	m_Pos[3] = Vector3(1.f, -1.f, 0.f);

	m_Pos[4] = Vector3(-1.f, 1.f, 1.f);
	m_Pos[5] = Vector3(1.f, 1.f, 1.f);
	m_Pos[6] = Vector3(-1.f, -1.f, 1.f);
	m_Pos[7] = Vector3(1.f, -1.f, 1.f);
}

CFrustum::~CFrustum()
{
}

void CFrustum::Update(Matrix matVP)
{
	// ������������� 8�� ��ġ, ��, ������ ��ǥ���� ���Ѵ�.
	Vector3	Pos[8];

	// ������� ���Ѵ�.
	matVP.Inverse();

	// �츮�� �ǵ���, ���� ������ �ִ� ��ǥ������, view * proj ����� ������� ���ؼ�
	// World �������� ��ȯ�Ϸ��� ���̴�.
	// �׷��ٸ� ������ �ش� ��ǥ������ ���� ������ ��ġ���־�� �Ѵٴ� ���ε�
	// ó�� ������, �ƴ�. ���ʿ� �ش� ������ ���� �������� ��ȯ ��Ű�� ������ �����µ� ���
	// �̷��� ������ ������ ���ΰ� ����.
	// - �ٷ�, ���ʿ� �� �����ڿ��� ���� ���� �� (��, NDC ��ǥ���� ���� ���� ��ǥ) �� �������� ������
	// - �̷� ������ ������ ���̴�.
	// - (���� ������, �������� ��ȯ ����, ��ǥ���� x,y �� -1���� 1 ����
	// z �� 0 ���� 1 ����)
	for (int i = 0; i < 8; ++i)
		Pos[i] = m_Pos[i].TransformCoord(matVP);

	// Left
	m_Plane[(int)Frustum_Plane_Dir::Left] = XMPlaneFromPoints(Pos[4].Convert(), Pos[0].Convert(),
		Pos[2].Convert());

	// Right
	m_Plane[(int)Frustum_Plane_Dir::Right] = XMPlaneFromPoints(Pos[1].Convert(), Pos[5].Convert(),
		Pos[7].Convert());

	// Top
	m_Plane[(int)Frustum_Plane_Dir::Top] = XMPlaneFromPoints(Pos[4].Convert(), Pos[5].Convert(),
		Pos[1].Convert());

	// Bottom
	m_Plane[(int)Frustum_Plane_Dir::Bottom] = XMPlaneFromPoints(Pos[2].Convert(), Pos[3].Convert(),
		Pos[7].Convert());

	// Near
	m_Plane[(int)Frustum_Plane_Dir::Near] = XMPlaneFromPoints(Pos[0].Convert(), Pos[1].Convert(),
		Pos[3].Convert());

	// Far
	m_Plane[(int)Frustum_Plane_Dir::Far] = XMPlaneFromPoints(Pos[5].Convert(), Pos[4].Convert(),
		Pos[6].Convert());
}

// https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=ya3344&logNo=221400519972
bool CFrustum::FrustumInPoint(const Vector3& Point)
{
	for (int i = 0; i < (int)Frustum_Plane_Dir::Max; ++i)
	{
		// m_Plane[i].Convert() : ax + by + cz + d = 0 ���� ���ǵǴ� ����̴�.
		// �ش� ���� Point.Convert() ��� �� ������ �Ÿ���
		// Point.Convert() ��� �� x1, y1, z1 �� �ش� ��� ������ �Ÿ�
		// ax1 + by1 + cz1 + d = k
		// �̰��� ������� ������ �Ǵ��ؾ� �Ѵ�.
		// (���� : d �� ���⼺ ��ȣ�̴�. ��鿡�� ���� ���������� ���� ������
		// Normal ���� ����� ���ٸ� + , �װ� �ƴ϶�� - �̴�)
		// k > 0 : ��� ���� (��� ��)
		// k = 0 : ��� ��
		// k < 0 : ��� ���� (��� ��)
		// ���� ������ ��� Normal ���͵���, Frustum ���ʿ���, �ٱ������� ���ϴ� ����
		// ���� �ش� ���� �� �ϳ��� ������ ����, �ش� ��� �ۿ� ��ġ�Ѵٴ� ��
		// ��, Frustum �ۿ� ��ġ�Ѵٴ� ���̴�.
		// �� ���, Culling �� �Ǿ�, ȭ�鿡 �������� �ȵǴ� �༮�̴�.
		float	Dot = XMVectorGetX(XMPlaneDotCoord(m_Plane[i].Convert(), Point.Convert()));

		if (Dot > 0.f)
			return true;
	}

	return false;
}

bool CFrustum::FrustumInSphere(const SphereInfo& Sphere)
{
	// true ��� ����, culling �� �Ǿ��ٴ� ��
	// ��, ȭ�鿡�� ������ �ʴ� �༮�̶�� �� 

	for (int i = 0; i < (int)Frustum_Plane_Dir::Max; ++i)
	{
		// �Ʒ��� ���, ����� �����Ŀ��� d �� �����ϴ� ������ ����ΰɱ� 
		// World Space �󿡼� ����� ���� ���̹Ƿ�, World Space ���� ����̶�� �ص� ���� ������ ?

		// XMPlaneDotCoord�� ���, point ~ plane ������ �Ÿ��� �����ϴµ� ���ȴ�.
		
		// ���� Vector3 ���� Dot Product ����� Return
		// ������ x,y,z,w ���� �� �����ϹǷ� GetX �� ���� �ϳ��� ���� �����´�.
		
		// World ���� �󿡼� ��� ~ ��. ������ �Ÿ��� ������ ���̴�.
		// XMPlaneDotCoord �� ����� ���, 0, ����
		// 0, �������, SphereInfo �� �߽��� Frustum ���ʿ� �ִٴ� �ǹ��̹Ƿ�
		// ����ü Culling�� ����� �ƴϴ�.
		// ������ �����, �ش� ����� ����� ���̴�.
		
		// ����� ���� ���ʹ�, ����ü ���� ���ϰ� �ִ�.
		
		// � ��ü�� BV ���� ����ü �ۿ� ��ġ�ϰ� �ִٰ� �غ���.
		// �ش� BV ���� �߽� ~ ��� ������ �Ÿ��� ���������� �۴ٸ�
		// BV ���� �Ϻκ���, ��� �ۿ���, ����ü �������� ������ �ִٴ� ��
		// �� ���� ����ü �ȿ� ������ ���̹Ƿ� �׷����� �Ѵ�.
		// ����, BV ���� �߽��� ����ü �ȿ� �����Ѵٸ� ?
		float	Dot = XMVectorGetX(XMPlaneDotCoord(m_Plane[i].Convert(), Sphere.Center.Convert()));

		if (Dot > Sphere.Radius)
			return true;
	}

	return false;
}
