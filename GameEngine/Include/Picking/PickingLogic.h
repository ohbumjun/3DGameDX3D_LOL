#pragma once
#include "../GameInfo.h"

class CPickingLogic
{
public:
	static bool Picking(class CGameObject*& result);
	static void Bresenham(int stR, int stC, int edR, int edC, std::vector<std::pair<int, int>>& vecP);
	static bool DDTPicking(class CGameObject* LandScapeObject, class CGameObject* Player, Vector3& PickedWorldPos);
	// �� 2�� , Ray Start ����, Ray ���� ������ �ش�
	// �׷���, �� 2���� ���� ���� ~ Ray ���� ���� �������� �����ش�.
	// ��, ��� �������� ���� ��ǥ�� �� ��ġ�ؾ� �Ѵ�.
	static bool GetIntersectPoints(const Vector3& StartPoint, const Vector3& EndPoint,
		const Vector3& RayStartPos, const Vector3& RayDir, std::vector<Vector3>& vecIntersects);
};

