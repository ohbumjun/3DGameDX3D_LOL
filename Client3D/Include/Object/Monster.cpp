
#include "Monster.h"
#include "MonsterAnimation.h"
#include "Component/ColliderSphere.h"
#include "Component/ColliderBox3D.h"

CMonster::CMonster()
{
	SetTypeID<CMonster>();
}

CMonster::CMonster(const CMonster& obj) :
	CGameObject(obj)
{
	m_Mesh = (CAnimationMeshComponent*)FindComponent("Mesh");
}

CMonster::~CMonster()
{
}

bool CMonster::Init()
{
	m_Mesh = CreateComponent<CAnimationMeshComponent>("MonsterMesh");

	m_Mesh->SetMesh("PlayerMesh");
	m_Mesh->SetReceiveDecal(false);
	m_Mesh->SetRelativeScale(0.02f, 0.02f, 0.02f);

	// Animation
	m_Mesh->CreateAnimationInstance<CMonsterAnimation>();
	m_Animation = (CMonsterAnimation*)m_Mesh->GetAnimationInstance();

	// Collider Box
	/*
	m_ColliderBox3D = CreateComponent<CColliderBox3D>("ColliderBox3D");
	m_Mesh->AddChild(m_ColliderBox3D);

	const Vector3& AnimComponentMeshSize = m_Mesh->GetMeshSize();
	const Vector3& MeshRelativeScale = m_Mesh->GetRelativeScale();

	// �Ʒ� ��ġ�� ���� (������ ���� ũ��� �̿� �°� ��������� �Ѵ�)
	// Vector3 ColliderLength = Vector3(
	// 	AnimComponentMeshSize.x * MeshRelativeScale.x,
	// 	AnimComponentMeshSize.y * MeshRelativeScale.y,
	// 	AnimComponentMeshSize.z * MeshRelativeScale.z
	// );
	Vector3 ColliderLength = Vector3(
		AnimComponentMeshSize.x * MeshRelativeScale.x * 0.5f,
		AnimComponentMeshSize.y * MeshRelativeScale.y,
		AnimComponentMeshSize.z * MeshRelativeScale.z
	);

	Vector3 ColliderCenter = Vector3(
		GetWorldPos().x,
		GetWorldPos().y + AnimComponentMeshSize.y * MeshRelativeScale.y * 0.5f,
		GetWorldPos().z
	);

	// Center ������ ���, �⺻������ Player �� WorldPos �� �߹����� ������.
	// ��, �ƹ� ó���� ������ ���� ���, Center �� �߹����� �����ٴ� �ǹ��̴�.
	// MeshSize y��ŭ 0.5 �÷��� Center �� ���� ���̴�.
	// �ش� ���� ������ �̿��ص� �ȴ�.
	m_ColliderBox3D->SetInfo(ColliderCenter, ColliderLength * 0.5f);
	*/


	// Collider Sphere
	m_ColliderSphere = CreateComponent<CColliderSphere>("ColliderSphere");
	m_Mesh->AddChild(m_ColliderSphere);
	m_ColliderSphere->SetCollisionProfile("Monster");
	const Vector3& AnimComponentMeshSize = m_Mesh->GetMeshSize();
	const Vector3& MeshRelativeScale = m_Mesh->GetRelativeScale();
	Vector3 ColliderCenter = Vector3(
		GetWorldPos().x,
		GetWorldPos().y + AnimComponentMeshSize.y * MeshRelativeScale.y * 0.5f,
		GetWorldPos().z
	);

	float ColliderRadiius = AnimComponentMeshSize.x * MeshRelativeScale.x;
	ColliderRadiius = AnimComponentMeshSize.y * MeshRelativeScale.y < ColliderRadiius ?
		AnimComponentMeshSize.y * MeshRelativeScale.y :ColliderRadiius;
	ColliderRadiius = AnimComponentMeshSize.z * MeshRelativeScale.z < ColliderRadiius ?
		AnimComponentMeshSize.z * MeshRelativeScale.z :ColliderRadiius;

	m_ColliderSphere->SetInfo(ColliderCenter, ColliderRadiius);


	return true;
}

void CMonster::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CMonster::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);
}

CMonster* CMonster::Clone()
{
	return new CMonster(*this);
}
