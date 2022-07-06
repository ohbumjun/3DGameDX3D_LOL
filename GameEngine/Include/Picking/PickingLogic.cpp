#include "PickingLogic.h"
#include "../Component/CameraComponent.h"
#include "../Component/ColliderBox3D.h"
#include "../Component/ColliderSphere.h"
#include "../Component/LandScape.h"
#include "../GameObject/SkyObject.h"
#include "../Input.h"
#include "../Collision/Collision.h"
#include "../Scene/SceneManager.h"

bool CPickingLogic::Picking(CGameObject*& result)
{
	CCameraComponent* Camera = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();

	// CInput ���� �ǽð� ������ִ� Ray ������ �� ���� �󿡼��� Ray
	// �Ʒ��� �Լ��� ���ؼ�, View ����� ������� ������ Ray
	// ��, World ���� ���� Ray �� �������ش�.
	// �ֳ��ϸ�, ���ʿ� �ǽð����� ������ Input ������ ray ������
	// �信���� ray �����̱� �����̴�.
	Ray	ray = CInput::GetInst()->GetRay(Camera->GetViewMatrix());

	const std::list<class CSceneComponent*>& RenderCompLists = CSceneManager::GetInst()->GetScene()->GetRenderComponentsList();

	auto	iter = RenderCompLists.begin();
	auto	iterEnd = RenderCompLists.end();

	Vector3	HitPoint;

	for (; iter != iterEnd; ++iter)
	{
		SphereInfo	Info = (*iter)->GetSphereInfo();

		if (CCollision::CollisionRayToSphere(HitPoint, ray, Info))
		{
			result = (*iter)->GetGameObject();
			return true;
		}
	}

	result = nullptr;

	return false;
}

void CPickingLogic::Bresenham(int stR, int stC, int edR, int edC, std::vector<std::pair<int, int>>& vecIdxP)
{
	// ���� 
	int x = stC;
	// ���� 
	int y = stR;

	int dx = edC - stC;
	int dy = edR - stR;

	int detP = 2 * dy - dx;

	while (x <= edC)
	{
		vecIdxP.push_back(std::make_pair(x, y));
		++x;

		if (detP < 0)
			detP = detP + 2 * dy;
		else
		{
			detP = detP + 2 * dy - 2 * dx;
			y++;
		}
	}
}

bool CPickingLogic::DDTPicking(CGameObject* LandScapeObject, CGameObject* Player, Vector3& PickedWorldPos)
{
	// LandScape Object �� �ƴ϶�� return
	CLandScape* LandScapeComponent = dynamic_cast<CLandScape*>(LandScapeObject->GetRootComponent());

	if (LandScapeComponent == nullptr)
		return false;

	// >> 1. Ray �� World Space �� �����ش�. (Ray ~ LandScape => World Space ���� ������ ���̴�.)
	CCameraComponent* Camera = CSceneManager::GetInst()->GetScene()->GetCameraManager()->GetCurrentCamera();
	Ray	ray = CInput::GetInst()->GetRay(Camera->GetViewMatrix());

	// >> 2. ���� xz �� �����Ѵ�. (ray�� ��ġ�� y pos �� 0 �� ����, Dir �� ���, xz ������ ���)
	// Ray �����
	// Vector3 RayOnLandScapeY = Vector3(ray.Pos.x, LandScapeWorldPos.y, ray.Pos.z) + Vector3(ray.Dir.x, 0.f, ray.Dir.z);
	Vector3 RayStPosOnLandScapeY = Vector3(ray.Pos.x, 0.f, ray.Pos.z);

	// x,z ��� �󿡼��� ���� ����
	Vector3 RayDirWithYZero = Vector3(ray.Dir.x, 0.f, ray.Dir.z);

	// �˻� ������, ������ ã�´�. -> �� ���� ���� �˰��� Ȱ���ϱ�
	Vector3 RayFinalCheckStPos, RayFinalCheckEdPos;

	// LandScape �� ���, ���� ������ �Ʒ��� �������� ���·� �̷���� �ִ�.
	// � ��ġ�� �������� �ʴ´ٸ�, min�� 0�� �ǰ�, max �� ũ�⸸ŭ ���õǾ� ���� ���̴�.
	// ��, WorldPos �� �簢�� ���� �ϴܿ� �дٰ� �����ϸ� �ȴ�.
	Vector3 LandScapeMin = LandScapeComponent->GetMin() * LandScapeComponent->GetRelativeScale() + LandScapeComponent->GetWorldPos();
	LandScapeMin.y = 0.f;

	Vector3 LandScapeMax = LandScapeComponent->GetMax() * LandScapeComponent->GetRelativeScale() + LandScapeComponent->GetWorldPos();
	LandScapeMax.y = 0.f;

	// Ray ������ , �簢�� 4�� �� ������ �������� ���ؾ� �Ѵ�.
	// ���� Ray �� ���� ������ End ������ ���ؾ� �Ѵ�. �ش� ���� ~ Ray Start ������ �̾ �ϳ��� ������
	// ������ �� �ֱ� �����̴�.
	// RayStart ���� RayDir �������� ��� �ÿ��� ������ ������ ��´�.
	// LandScape ũ���� * 10�踸ŭ �ÿ��� ���� �� (��� LandScape ���� �ۿ� ��ġ�ؾ� �Ѵ�.)
	Vector3 TempRayCheckEdPos = RayStPosOnLandScapeY + RayDirWithYZero * LandScapeComponent->GetMeshSize() * LandScapeComponent->GetRelativeScale() * 10.f;
	TempRayCheckEdPos.y = 0.f;
	
	std::vector<Vector3> vecIntersects;

	// �Ʒ� ����
	GetIntersectPoints(LandScapeMin, Vector3(LandScapeMax.x, 0.f, LandScapeMin.z), 
		RayStPosOnLandScapeY, TempRayCheckEdPos, vecIntersects);
	// ���� ����
	GetIntersectPoints(LandScapeMin, Vector3(LandScapeMin.x, 0.f, LandScapeMax.z), 
		RayStPosOnLandScapeY, TempRayCheckEdPos, vecIntersects);
	// ���� ����
	GetIntersectPoints(Vector3(LandScapeMin.x, 0.f, LandScapeMax.z), LandScapeMax, 
		RayStPosOnLandScapeY, TempRayCheckEdPos, vecIntersects);
	// ������ ���� 
	GetIntersectPoints(Vector3(LandScapeMax.x, 0.f, LandScapeMin.z), LandScapeMax,
		RayStPosOnLandScapeY, TempRayCheckEdPos, vecIntersects);

	// ã�Ƴ� �������� LandScape �ۿ� �ִ��� Ȯ���Ѵ�.
	auto iterSt = vecIntersects.begin();
	auto iterEd = vecIntersects.end();

	for (; iterSt != iterEd;)
	{
		if ((*iterSt).x < LandScapeMin.x || (*iterSt).z < LandScapeMin.z || 
			(*iterSt).x > LandScapeMax.x || (*iterSt).z > LandScapeMax.z)
		{
			vecIntersects.erase(iterSt);
		}
		++iterSt;
	}

	// 1) Ray  �������� LandScape �ȿ� �ִٸ�, 1���� �������� ���;� �ϰ�, �װ��� �ٷ� End
	// 2) Ray �������� LandScape �ۿ� �ִٸ�, �������� ���, ������ 2���� �� ���;� �Ѵ�.
	// 3) �׿��� ��� -> Ray�� LandScape �� ������ �ʴ´ٴ� ���̴�.
	if (vecIntersects.size() <= 0)
		return false;

	bool IsRayStInsideLandScape = false;

	if (RayStPosOnLandScapeY.x >= LandScapeMin.x && RayStPosOnLandScapeY.z >= LandScapeMin.z &&
		RayStPosOnLandScapeY.x <= LandScapeMax.x && RayStPosOnLandScapeY.z <= LandScapeMax.z)
	{
		// LandScape ���ʿ� Ray�� Start ��ġ�� �ִٴ� ���̴�.
		IsRayStInsideLandScape = true;
		RayFinalCheckStPos = RayStPosOnLandScapeY;
	}

	// Ray Start �� LandScape ���ʿ� �����Ѵٸ�, ������ 1��
	if (IsRayStInsideLandScape == true)
	{
		// ���� �ɸ��� �ȵǴ� �� �ƴѰ� ?
		if (vecIntersects.size() != 1)
			return false;

		RayFinalCheckEdPos = vecIntersects[0];
	}
	
	// Ray Start �� LandScape�ۿ� �����Ѵٸ� -> ������ 2��
	else
	{
		// ���������� ���� �ɸ��� �ȵǴ� �� �ƴѰ� ?
		if (vecIntersects.size() != 2)
			return false;

		// X ���� �� ���� ����, St P �� ������ ���̴�.
		if (vecIntersects[0].x < vecIntersects[1].x)
		{
			RayFinalCheckStPos = vecIntersects[0];
			RayFinalCheckEdPos = vecIntersects[1];
		}
		else
		{
			RayFinalCheckStPos = vecIntersects[1];
			RayFinalCheckEdPos = vecIntersects[0];
		}
	}

	// >> 3. Bresenham �˰����� �̿��ؼ�, �ش� Ray�� �������� LandScape ���� ���� ����� �̾Ƴ���
	// St, Ed ������ ���ؼ� LandScape ���� 2���� ��ǥ���� ���Ѵ�.
	// ���� ���, CountX,Z �� ���� 129 �����
	// ���� �簢�� ������ ���� 128, 128���̴�.
	float LandScapeSizeX = (LandScapeMax.x - LandScapeMin.x) / (float)(LandScapeComponent->GetCountX() - 1);
	float LandScapeSizeZ = (LandScapeMax.z - LandScapeMin.z) / (float)(LandScapeComponent->GetCountZ() - 1);

	int RayStartXIdx = (int)((RayFinalCheckStPos.x - LandScapeMin.x) / LandScapeSizeX);
	int RayStartZIdx = (int)((RayFinalCheckStPos.z - LandScapeMin.z) / LandScapeSizeZ);

	int RayEndXIdx = (int)((RayFinalCheckEdPos.x - LandScapeMin.x) / LandScapeSizeX);
	int RayEndZIdx = (int)((RayFinalCheckEdPos.z - LandScapeMin.z) / LandScapeSizeZ);

	// LandScape 4���� �� ������ ���� ���� �������� �����Ѵ�.
	std::vector<std::pair<int, int>> vecLandScapeRayGoingThroughIdx;
	vecLandScapeRayGoingThroughIdx.reserve((size_t)LandScapeComponent->GetCountX() * (size_t)LandScapeComponent->GetCountZ());
	
	Bresenham(RayStartXIdx, RayStartZIdx, RayEndXIdx, RayEndZIdx, vecLandScapeRayGoingThroughIdx);

	// Ȥ�ó� �������� Idx ������ �������� �ʴ´ٸ� Return -> ���⿡ �ɸ��� �ȵǴ� �� �ƴѰ� ?
	if (vecLandScapeRayGoingThroughIdx.size() <= 0)
		return false;

	// >> 4. �ش� ���� �ȿ� �ִ� �ﰢ�� ����� �̾Ƴ���.
	// vecRayGoingThroughIdx �ȿ� ����ִ� Idx ������, ���� �ϴ� -> ������ ��� �������� �����ϴ�
	// LandScape �簢�� ������ Idx �����̴�.

	// vecTriangleRayGoingThroughIdx �� ���, ���� ��� -> ������ �ϴ� �������� �������� �ﰢ�� �Ǻ� ���� Idx
	// �ﰢ�� - Ray ���� �Ÿ� , �ﰢ�� Idx . �� ���� �������� ���� ���̴�.
	// ����, �ﰢ�� - Ray ���� �Ÿ�  �� �������� �������� �������ֱ� ���ؼ�, "�ﰢ�� - Ray ���� �Ÿ�" �� ���� ��´�.
	std::vector<int> vecTriangleRayGoingThroughIdx;

	for (const auto &[XIdx, ZIdx] : vecLandScapeRayGoingThroughIdx)
	{ 
		// Ray	ray = CInput::GetInst()->GetRay(Camera->GetViewMatrix());
		auto IntersectResult = LandScapeComponent->CheckRayIntersectsTriangle(XIdx, ZIdx, ray.Pos, ray.Dir);

		if (IntersectResult.has_value())
		{
			float DistToTriangle = IntersectResult.value();
			vecTriangleRayGoingThroughIdx.push_back(DistToTriangle);
		}
	}

	// 4. �ش� �ﰢ�� ��� �߿��� Ray �� ���� ������ ��ġ�� �ﰢ�� ������ �̾Ƴ���.
	// Ȥ�ó� �浹�ϴ� �ﰢ�� ������ ���ٸ� Break -> �̰��� ���ɼ� �ִ�. Bounding Volume ���� �ε��� ��
	// ���� Traingle ����� �ε����� ���� ���� �ֱ� �����̴�.
	if (vecLandScapeRayGoingThroughIdx.size() <= 0)
		return false;

	float DistToTriangle = vecLandScapeRayGoingThroughIdx[0].second;

	PickedWorldPos = ray.Pos + ray.Dir * DistToTriangle;

	return true;
}

// https://devvcxxx.tistory.com/38
bool CPickingLogic::GetIntersectPoints(const Vector3& StartPoint, const Vector3& EndPoint, 
	const Vector3& RayStartPos, const Vector3& RayEndPos,
	std::vector<Vector3>& vecIntersects)
{
	// BP1 : StartPoint, 
	// BP2 : EndPoint
	// AP1 : RayStartPos
	// AP2 : RayEndPos
	float t;
	float s;

	// �и�
	// double under = (BP2.y - BP1.y) * (AP2.x - AP1.x) - (BP2.x - BP1.x) * (AP2.y - AP1.y);
	float under = (EndPoint.z - StartPoint.z) * (RayEndPos.x - RayStartPos.x) - (EndPoint.x - StartPoint.x) * (RayEndPos.z - RayStartPos.z);
	
	// �и��� 0 �̶�� �ǹ̴� �� ���� �����ϴٴ� �ǹ��̴�.
	if (under == 0)
		return false;

	// double _t = (BP2.x - BP1.x) * (AP1.y - BP1.y) - (BP2.y - BP1.y) * (AP1.x - BP1.x);
	// double _s = (AP2.x - AP1.x) * (AP1.y - BP1.y) - (AP2.y - AP1.y) * (AP1.x - BP1.x);
	float _t = (EndPoint.x - StartPoint.x) * (RayStartPos.z - StartPoint.z) - (EndPoint.z - StartPoint.z) * (RayStartPos.x - StartPoint.x);
	float _s = (RayEndPos.x - RayStartPos.x) * (RayStartPos.z - StartPoint.z) - (RayEndPos.z - RayStartPos.z) * (RayStartPos.x - StartPoint.x);

	// t, s �� �ٷ�, �� ������ �������̴�.
	t = _t / under;
	s = _s / under;

	// t, s �� 0���� 1������ ����߸�, �� ���� ���� ���̿� ������ �����Ѵٴ� �ǹ��̴�.
	if (t < 0.0 || t>1.0 || s < 0.0 || s>1.0)
		return false;

	// ���ڰ��� �Ѵ� 0�̶�� �ǹ̴�, �� ������ ���� �����̶�� �ǹ��̴�.
	if (_t == 0 && _s == 0)
		return false;

	float IntersectX = RayStartPos.x + t * (float)(RayEndPos.x - RayStartPos.x);
	float IntersectZ = RayStartPos.z + t * (float)(RayEndPos.z - RayStartPos.z);

	vecIntersects.push_back(Vector3(IntersectX, 0.f, IntersectZ));

	return true;
}
