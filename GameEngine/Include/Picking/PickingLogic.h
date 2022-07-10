#pragma once
#include "../GameInfo.h"

struct Polygon
{
	Vector3 v1;
	Vector3 v2;
	Vector3 v3;
};

class CPickingLogic
{
public:
	static bool Picking(class CGameObject*& result);
	static void Bresenham(int stR, int stC, int edR, int edC, 
		std::vector<std::pair<int, int>>& vecP);
	static bool DDTPicking(class CGameObject* LandScapeObject, class CGameObject* Player, Vector3& PickedWorldPos);
	static bool GetLineIntersect(const Vector3& StartPoint, const Vector3& EndPoint,
		const Vector3& RayStartPos, const Vector3& RayEndPos,
		std::vector<Vector3>& vecIntersects);
	// �� 2�� , Ray Start ����, Ray ���� ������ �ش�
	// �׷���, �� 2���� ���� ���� ~ Ray ���� ���� �������� �����ش�.
	// ��, ��� �������� ���� ��ǥ�� �� ��ġ�ؾ� �Ѵ�.
	static bool CheckRayTriangleIntersect(
		const Vector3& rayOrig, const Vector3& dir,
		const Vector3& v0, const Vector3& v1, const Vector3& v2,
		float& IntersectDist, Vector3& IntersectPoint);
};

