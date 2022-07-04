
#include "DecalObj.h"

#include "Component/PickingLayerBox3D.h"

CDecalObj::CDecalObj()
{
	SetTypeID<CDecalObj>();
}

CDecalObj::CDecalObj(const CDecalObj& obj) :
	CGameObject(obj)
{
	m_Decal = (CDecalComponent*)FindComponent("Decal");
}

CDecalObj::~CDecalObj()
{
}

bool CDecalObj::Init()
{
	m_Decal = CreateComponent<CDecalComponent>("Decal");

	SetWorldPos(20.f, 0.f, 13.f);

	// Culling Area
	const Vector3& AnimComponentMeshSize = m_RootComponent->GetMeshSize();
	const Vector3& MeshRelativeScale = m_RootComponent->GetRelativeScale();

	// �Ʒ� ��ġ�� ���� (������ ���� ũ��� �̿� �°� ��������� �Ѵ�)
	// Vector3 ColliderLength = Vector3(
	// 	AnimComponentMeshSize.x * MeshRelativeScale.x,
	// 	AnimComponentMeshSize.y * MeshRelativeScale.y,
	// 	AnimComponentMeshSize.z * MeshRelativeScale.z
	// );
	Vector3 ColliderLength = Vector3(
		AnimComponentMeshSize.x * MeshRelativeScale.x ,
		AnimComponentMeshSize.y * MeshRelativeScale.y,
		AnimComponentMeshSize.z * MeshRelativeScale.z
	);

	m_CullingArea3D = CreateComponent<CPickingLayerBox3D>("ColliderBox3D");
	m_RootComponent->AddChild(m_CullingArea3D);
	m_CullingArea3D->SetLength(ColliderLength * 0.5f);

	return true;
}

void CDecalObj::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CDecalObj::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CDecalObj* CDecalObj::Clone()
{
	return new CDecalObj(*this);
}
