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

// ���Ⱑ ����϶��� ����Ѵ�.
// �Ϲ� Bresenham�� �Ϲ������� x�� 1����
// Ư�� �Ǻ����� �̿��ؼ� x�� 1 ������ ��, y�� �״�� �ӹ��� ������, y�� 1���������� ����
void CPickingLogic::Bresenham(int stR, int stC, int edR, int edC, std::vector<std::pair<int, int>>& vecIdxP)
{
	// ���Ⱑ 1��������, 1���� ū���� ����
	// detP �� ���ϴ� ����� �ٸ���.

	// ���� (��)
	int C = stC;
	// ���� (��)
	int R = stR;

	int dC = edC - stC;
	int dR = edR - stR;

	// ���Ⱑ 1 ������ �����
	if (dR <= dC)
	{
		int detP = 2 * (dR - dC);

		while (C <= edC)
		{
			// ����, ���� (��, ��)
			// ��, (z,x) ������ ��ȯ�Ѵ�.
			vecIdxP.push_back(std::make_pair(R, C));
			++C;

			if (detP < 0)
				detP = detP + 2 * dR;
			else
			{
				detP = detP + 2 * (dR - dC);
				R++;
			}
		}
	}
	// ���Ⱑ 1 ���� ū �����
	else
	{
		int detP = 2 * (dC - dR);

		while (R <= edR)
		{
			// ����, ���� (��, ��)
			// ��, (z,x) ������ ��ȯ�Ѵ�.
			vecIdxP.push_back(std::make_pair(R, C));
			++R;

			if (detP < 0)
				detP = detP + 2 * dC;
			else
			{
				detP = detP + 2 * (dC - dR);
				C++;
			}
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
	ray.Dir.Normalize();
		
	// >> 2. ���� xz �� �����Ѵ�. (ray�� ��ġ�� y pos �� 0 �� ����, Dir �� ���, xz ������ ���)
	// Ray �����
	// Vector3 RayOnLandScapeY = Vector3(ray.Pos.x, LandScapeWorldPos.y, ray.Pos.z) + Vector3(ray.Dir.x, 0.f, ray.Dir.z);
	Vector3 RayStPosWithYZero = Vector3(ray.Pos.x, 0.f, ray.Pos.z);

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
	// LandScape ũ���� * 1000�踸ŭ �ÿ��� ���� �� (��� LandScape ���� �ۿ� ��ġ�ؾ� �Ѵ�.)
	Vector3 TempRayCheckEdPos = RayStPosWithYZero + RayDirWithYZero * LandScapeComponent->GetMeshSize() * LandScapeComponent->GetRelativeScale() * 1000.f;
	TempRayCheckEdPos.y = 0.f;
	
	std::vector<Vector3> vecIntersects;

	// �Ʒ� ����
	bool IntersectLineToLineResult = GetLineIntersect(LandScapeMin, Vector3(LandScapeMax.x, 0.f, LandScapeMin.z),
		RayStPosWithYZero, TempRayCheckEdPos, vecIntersects);
	// ���� ����
	IntersectLineToLineResult = GetLineIntersect(LandScapeMin, Vector3(LandScapeMin.x, 0.f, LandScapeMax.z),
		RayStPosWithYZero, TempRayCheckEdPos, vecIntersects);
	// ���� ����
	IntersectLineToLineResult = GetLineIntersect(Vector3(LandScapeMin.x, 0.f, LandScapeMax.z), LandScapeMax,
		RayStPosWithYZero, TempRayCheckEdPos, vecIntersects);
	// ������ ���� 
	IntersectLineToLineResult = GetLineIntersect(Vector3(LandScapeMax.x, 0.f, LandScapeMin.z), LandScapeMax,
		RayStPosWithYZero, TempRayCheckEdPos, vecIntersects);

	// ã�Ƴ� �������� LandScape �ۿ� �ִ��� Ȯ���Ѵ�.
	auto iterSt = vecIntersects.begin();
	auto iterEd = vecIntersects.end();

	// ���� ex) MaxZ �� 128�ε�, 128.0001 �̷� ���ڰ� ���ö��� �ִ�.
	float Offset = 0.01f;

	for (; iterSt != iterEd;)
	{
		if ((*iterSt).x < LandScapeMin.x - Offset || (*iterSt).z < LandScapeMin.z - Offset ||
			(*iterSt).x > LandScapeMax.x + Offset || (*iterSt).z > LandScapeMax.z + Offset)
		{
			iterSt = vecIntersects.erase(iterSt);
			continue;
		}
		++iterSt;
	}

	// 1) Ray  �������� LandScape �ȿ� �ִٸ�, 1���� �������� ���;� �ϰ�, �װ��� �ٷ� End
	// 2) Ray �������� LandScape �ۿ� �ִٸ�, �������� ���, ������ 2���� �� ���;� �Ѵ�.
	// 3) �׿��� ��� -> Ray�� LandScape �� ������ �ʴ´ٴ� ���̴�.
	if (vecIntersects.size() <= 0)
		return false;

	bool IsRayStInsideLandScape = false;

	if (RayStPosWithYZero.x >= LandScapeMin.x && RayStPosWithYZero.z >= LandScapeMin.z &&
		RayStPosWithYZero.x <= LandScapeMax.x && RayStPosWithYZero.z <= LandScapeMax.z)
	{
		// LandScape ���ʿ� Ray�� Start ��ġ�� �ִٴ� ���̴�.
		IsRayStInsideLandScape = true;
		RayFinalCheckStPos = RayStPosWithYZero;
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

	// X ���� �� ���� ����, St P �� ������ ���̴�.
	// ��, �� ���ʿ� �ִ� �༮ (���� �� ����)
	// �̷��߸�, �Ʒ��� BresenHam �˰����� ����ȴ�. (���� �� ���� �༮����, ���� �� ū �༮���� ���� ��)
	if (RayFinalCheckEdPos.x < RayFinalCheckStPos.x)
	{
		Vector3 Temp = RayFinalCheckEdPos;
		RayFinalCheckEdPos = RayFinalCheckStPos;
		RayFinalCheckStPos = RayFinalCheckEdPos;
	}

	// >> 3. Bresenham �˰����� �̿��ؼ�, �ش� Ray�� �������� LandScape ���� ���� ����� �̾Ƴ���
	// St, Ed ������ ���ؼ� LandScape ���� 2���� ��ǥ���� ���Ѵ�.
	// ���� ���, CountX,Z �� ���� 129 �����
	// ���� �簢�� ������ ���� 128, 128���̴�.
	float LandScapeSizeX = (LandScapeMax.x - LandScapeMin.x) / (float)(LandScapeComponent->GetCountX() - 1);
	float LandScapeSizeZ = (LandScapeMax.z - LandScapeMin.z) / (float)(LandScapeComponent->GetCountZ() - 1);

	// X Idx : ��
	// Z Idx : ��
	int RayStartXIdx = (int)((RayFinalCheckStPos.x - LandScapeMin.x) / LandScapeSizeX);
	int RayStartZIdx = (int)((RayFinalCheckStPos.z - LandScapeMin.z) / LandScapeSizeZ);

	int RayEndXIdx = (int)((RayFinalCheckEdPos.x - LandScapeMin.x) / LandScapeSizeX);
	int RayEndZIdx = (int)((RayFinalCheckEdPos.z - LandScapeMin.z) / LandScapeSizeZ);

	// ���� ���, CountX, CountZ �� 129, 129 ��� �Ѵٸ�
	// ���� �簢�� ������ 128 * 128 ���̰�
	// ������ Idx ������ 0 ~ 127 �� �Ǿ�� �Ѵ�.
	// ���� �̿� �°� ������ ��� �Ѵ�.
	if (RayStartXIdx >= LandScapeComponent->GetCountX() - 1)
		RayStartXIdx = LandScapeComponent->GetCountX() - 2;

	if (RayEndXIdx >= LandScapeComponent->GetCountX() - 1)
		RayEndXIdx = LandScapeComponent->GetCountX() - 2;

	if (RayStartZIdx >= LandScapeComponent->GetCountZ() - 1)
		RayStartZIdx = LandScapeComponent->GetCountZ() - 2;

	if (RayEndZIdx >= LandScapeComponent->GetCountZ() - 1)
		RayEndZIdx = LandScapeComponent->GetCountZ() - 2;

	// LandScape 4���� �� ������ ���� ���� �������� �����Ѵ�.
	std::vector<std::pair<int, int>> vecLandScapeRayGoingThroughIdx;
	vecLandScapeRayGoingThroughIdx.reserve((size_t)LandScapeComponent->GetCountX() * (size_t)LandScapeComponent->GetCountZ());

	// Z: ��
	// X : ��
	// (Z, X) ���� ��ȯ
	// 1) RaySt -> Ray End �� ���Ⱑ �����ϴ� ���¶��, ������ Bresenham �˰����� �״�� ����ص� �ȴ�.
	// 2) �ݸ�, ���Ⱑ �����ϴ� ���¶��, ������ ������, Bresenham�� ������ ��, �����ؾ� �Ѵ�.
	// - ��, ��ġ ���� ��ܿ���, ������ �ϴ� ������ �����ϴ� ���� (���ʿ��� �������� �ٶ� ���·� ����)
	if (RayStartZIdx <= RayEndZIdx)
	{
		// ���� ��, ���� ��, �� ��, �� ��
		// Bresenham(int stR, int stC, int edR, int edC, std::vector<std::pair<int, int>>& vecIdxP)
		Bresenham(RayStartZIdx, RayStartXIdx, RayEndZIdx, RayEndXIdx, vecLandScapeRayGoingThroughIdx);
	}
	else
	{
		// ���� 1
		/* (Before)
		2,0 / 2,1 / 2,2 / 2,3 / 2,4
		1,0 / 1,1 / 1,2 / 1,3 / 1,4
		0,0 / 0,1 / 0,2 / 0,3 / 0,4
		*/

		/* (After)
		0,0 / 1,0 / 2.0 / 3,0 / 4,0
		0,1 / 1,1 / 2,1 / 3,1 / 4,1
		0,2 / 1,2 / 2,2 / 3,2 / 4,3
		*/

		// ���� 2
		/* (Before)
		4,0 / 4,1 / 4,2
		3,0 / 3,1 / 3.2
		2,0 / 2,1 / 2,2
		1,0 / 1,1 / 1,2
		0,0 / 0,1 / 0,2
		*/

		/* (After)
		0,0 / 1,0 / 2.0
		0,1 / 1,1 / 2,1
		0,2 / 1,2 / 2,2
		0,3 / 1,3 / 2,3
		0,4 / 1,4 / 2,4
		*/

		// (Before -> After)
		// �ð�������� 90�� ������ ���� (����, 90�� ������ ���� �ƴϴ�, ��������� ��� ���� ������ �޶��� �� �ֱ� ����)
		// (bR, bC) , �� R��, �� C��
		// 1) �� -> ��
		// 2) (�� �� - 1) - ���� �� -> ��
		int ChangedStRowIdx = RayStartXIdx;
		int ChangedStColIdx = (LandScapeComponent->GetCountZ() - 1) - RayStartZIdx;

		int ChangedEdRowIdx = RayEndXIdx;
		int ChangedEdColIdx = (LandScapeComponent->GetCountZ() - 1) - RayEndZIdx;

		// ���� ��,�� - �� ��,��
		Bresenham(ChangedStRowIdx, ChangedStColIdx, ChangedEdRowIdx, ChangedEdColIdx, vecLandScapeRayGoingThroughIdx);

		// �׸��� �� (��,��) ������ �� �ٽ� �ٲ���� �Ѵ�. (After -> Before)
		// 1) �� -> ��
		// 2) (�� �� - 1) - ���� �� -> ��
		for (int i = 0; i < vecLandScapeRayGoingThroughIdx.size(); ++i)
		{
			// �� , �� (Z, X)
			auto [ZIdx, XIdx] = vecLandScapeRayGoingThroughIdx[i];
			vecLandScapeRayGoingThroughIdx[i] = std::make_pair(LandScapeComponent->GetCountX() - 1- XIdx,ZIdx);
		}
	}


	// Ȥ�ó� �������� Idx ������ �������� �ʴ´ٸ� Return -> ���⿡ �ɸ��� �ȵǴ� �� �ƴѰ� ?
	if (vecLandScapeRayGoingThroughIdx.size() <= 0)
		return false;

	// >> 4. �ش� ���� �ȿ� �ִ� �ﰢ�� ����� �̾Ƴ���.
	// vecRayGoingThroughIdx �ȿ� ����ִ� Idx ������, ���� �ϴ� -> ������ ��� �������� �����ϴ�
	// LandScape �簢�� ������ Idx �����̴�.

	// vecTriangleRayGoingThroughIdx �� ���, ���� ��� -> ������ �ϴ� �������� �������� �ﰢ�� �Ǻ� ���� Idx
	// ��, LandScape �� �簢�� ���� Idx ������ , �ﰢ�� Idx ������ �ٸ��� ������, �߰��� ���� ������ ���ľ� �Ѵ�.
	// �ﰢ�� - Ray ���� �Ÿ� , �ﰢ�� Idx . �� ���� �������� ���� ���̴�.
	// ����, �ﰢ�� - Ray ���� �Ÿ�  �� �������� �������� �������ֱ� ���ؼ�, "�ﰢ�� - Ray ���� �Ÿ�" �� ���� ��´�.
	std::vector<float> vecTriangleRayDist;

	// Z: �� Idx, X : �� Idx
	for (const auto &[ZIdx, XIdx] : vecLandScapeRayGoingThroughIdx)
	{ 
		// Ư�� ���ھ��� 2�� �ﰢ�� �߿��� �浹�ϴ� �ﰢ���� �ִ��� �˻��Ѵ�.
		auto IntersectResult = LandScapeComponent->CheckRayIntersectsTriangleInLandScape(XIdx, ZIdx,
			ray.Pos, ray.Dir);

		if (IntersectResult.has_value())
		{
			float DistToTriangle = IntersectResult.value();
			vecTriangleRayDist.push_back(DistToTriangle);
		}
	}

	// 4. �ش� �ﰢ�� ��� �߿��� Ray �� ���� ������ ��ġ�� �ﰢ�� ������ �̾Ƴ���.
	// Ȥ�ó� �浹�ϴ� �ﰢ�� ������ ���ٸ� Break -> �̰��� ���ɼ� �ִ�. Bounding Volume ���� �ε��� ��
	// ���� Traingle ����� �ε����� ���� ���� �ֱ� �����̴�.
	if (vecTriangleRayDist.size() <= 0)
		return false;

	std::sort(vecTriangleRayDist.begin(), vecTriangleRayDist.end());

	float DistToTriangle = vecTriangleRayDist[0];

	PickedWorldPos = ray.Pos + ray.Dir * DistToTriangle;

	return true;
}

// https://devvcxxx.tistory.com/38
bool CPickingLogic::GetLineIntersect(const Vector3& StartPoint, const Vector3& EndPoint,
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

bool CPickingLogic::CheckRayTriangleIntersect(
	const Vector3& rayOrig, const Vector3& rayDir,
	const Vector3& v0, const Vector3& v1, const Vector3& v2,
	float& IntersectDist, Vector3& IntersectPoint)
{
	// 1�ܰ�. ����, ��� ~ ����. �� ���� �������� ���θ� ���ɴ�.
	// ����� Normal ���� ����Ѵ�.
	// compute plane's normal
	Vector3 Edge1 = v1 - v0;
	Vector3 Edge2 = v2 - v0;

	// Normalize �� �ʿ�� ����.
	Vector3 NormalV = Edge1.Cross(Edge2);  //N 

	// ���� 0
	float kEpsilon = 0.00001;

	// ����� Normal Vector ��, Ray �� Dir�� Dot �����
	// ���� 0�̶��, ���� ���� �����̶�� ��
	// �ݴ�� ���ϸ�, ���� Ray �� ���� �����ϴٴ� ��
	float NormalVDotRayDir = NormalV.Dot(rayDir);
	
	if (abs(NormalVDotRayDir) < kEpsilon)  //almost 0 
		return false;  //they are parallel so they don't intersect ! 

	// 2�ܰ� : ������ ������ ���ϱ� 
	// Ax + By + Cz + D = 0 �̶�� ����� �����Ŀ���
	// D �� ���ϱ� 
	// Normal Vector �� ���Ұ� * ��� �� ���� �������� ���Ұ�
	float PlaneDist = NormalV.Dot(v0) * -1;

	// Ray ���� �������� �Ÿ��� ���Ѵ�.
	// Ax + By + Cz + D ����
	// x,y,z �� Ray ������ (StartPos + IntersectDist * RayDir) �� ����
	// �̸� ����, IntersectDist (������) ���� �����س� �� �ִ�.
	// compute t (equation 3)
	IntersectDist = -1 * (NormalV.Dot(rayOrig) + PlaneDist) / NormalVDotRayDir;

	// 3�ܰ� : Ray �� �������� Triangle ���� �տ� �ִ��� Ȯ��
	// ��, Ray Dir ���� �ݴ� �ʿ� Triangle�� ��ġ�Ѵٸ�
	// IntersectDist �� ������ ���� ���̴�.
	if (IntersectDist < 0) 
		return false; 

	// 4�ܰ� : ������ ��ǥ ���ϱ� 
	IntersectPoint = rayOrig + Vector3(rayDir.x * IntersectDist, rayDir.y * IntersectDist, rayDir.z * IntersectDist);
	
	// 5�ܰ� : �������� �ﰢ�� ���� �����ϴ��� Ȯ��
	Vector3 CkEdge = v1 - v0;
	Vector3 CkPointEdge = IntersectPoint - v0;
	Vector3 CkCrossResult = CkEdge.Cross(CkPointEdge);

	if (NormalV.Dot(CkCrossResult) < 0)
		return false;

	CkEdge = v2 - v1;
	CkPointEdge = IntersectPoint - v1;
	CkCrossResult = CkEdge.Cross(CkPointEdge);

	if (NormalV.Dot(CkCrossResult) < 0)
		return false;

	CkEdge = v0 - v2;
	CkPointEdge = IntersectPoint - v2;
	CkCrossResult = CkEdge.Cross(CkPointEdge);

	if (NormalV.Dot(CkCrossResult) < 0)
		return false;

	return true;  //this ray hits the triangle 
}

/*


*/