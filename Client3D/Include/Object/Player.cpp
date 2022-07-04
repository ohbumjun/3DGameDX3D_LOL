
#include "Player.h"
#include "PlayerAnimation.h"
#include "Input.h"
#include "Scene/Scene.h"
#include "Scene/Navigation3DManager.h"
#include "Weapon.h"
#include "Component/ColliderBox3D.h"
#include "Component/ColliderSphere.h"
#include "Component/PickingLayerBox3D.h"

// 1. �� Player �� SphereInfo.Center �� 0�� �ƴ϶� ������ ���õɱ� ? (y��ǥ)
// -> �Ʒ����� m_Mesh->SetMesh(PlayerMesh) �� �ϸ鼭, Load�� fbx Mesh ������ �����Ѵ�.
// -> �̶�, �ش� Mesh ���� min, max ������ �ִµ�, min�� 0���� �����ϴ� ���� �ƴϴ�. ���� �׶��׶� �ٸ� min, max �� ���õǾ� �ִ�.
// -> �̿� ���� SphereInfo.Center �� �ٸ��� ���õǴ� ���̴�.

// 2. �׷��� �� Player �� �߹��� min �� �ƴ϶�, Center ���� �����ϴ� ���ϱ� ?
// ��, �� SphereInfo.Center �� Player �� �߹����� ���õǴ� ���ϱ� ?
// -> ���� Ʋ�ȴ�. Player �� �߹���, Player �� World Pos �̴�. �׸��� �� ���� ó���� (0.f, 0.f, 0.f) �̴�
// -> �ݸ�, SphereInfo.Center �� y���� �̹��ϰ� -0.34 �̴�.
// -> ��, Mesh �� Min ��ü�� 0���� �����ϴ� ���� �ƴϱ� ������ SphereInfo.Center ��, ���� ��ü�� �߰� ��ġ�� ��ġ���� �ʴ� ��
// -> �ݸ�, LandScape �� ���, Min�� 0���� �����ϰ� �ڵ尡 �Ǿ� �ִ�. �׷��� Center �� ���� �߰� ��ġ�� ��ġ�ϴ� ��

// 3. ��� �ϸ� �ϰ��ǰ� Center ������ ������ �� ������ ?
// -> ��� min �� 0 ���� �����ϵ��� �������ָ� �Ǵ� �� �ƴұ� ?
// -> �ƴϴ�. ���� Mesh ������ �״�� �����ϴ� ���� ���� �� ����.
// -> ���� SphereInfo.Center ������ WorldPos �� Mesh ũ�⿡ �°� �������ָ� �� �� ����.
// -> Animation Mesh Ȥ�� Static Mesh Component �� ���ؼ��� �����ϸ� �� �� ����.
// -> 

CPlayer::CPlayer()
{
	SetTypeID<CPlayer>();
}

CPlayer::CPlayer(const CPlayer& obj)	:
	CGameObject(obj)
{
	m_Mesh = (CAnimationMeshComponent*)FindComponent("Mesh");
	m_Arm = (CArm*)FindComponent("Arm");
	m_Camera = (CCameraComponent*)FindComponent("Camera");
	m_NavAgent = (CNavAgent*)FindComponent("NavAgent");
}

CPlayer::~CPlayer()
{
}

bool CPlayer::Init()
{
	m_Mesh = CreateComponent<CAnimationMeshComponent>("PlayerMesh");
	m_Arm = CreateComponent<CArm>("Arm");
	m_Camera = CreateComponent<CCameraComponent>("Camera");
	m_NavAgent = CreateComponent<CNavAgent>("NavAgent");

	// Arm, Camera
	m_Mesh->AddChild(m_Arm);
	m_Arm->AddChild(m_Camera);

	m_Camera->SetInheritRotX(true);
	m_Camera->SetInheritRotY(true);
	m_Camera->SetInheritRotZ(true);

	// Set Mesh �� ����߸� MeshSize�� ���� (SetMeshSize)
	// - Root Component �� Animation Mesh Component �� Transform �� MeshSize �� ���� ������ ����ְ� �ȴ�. 
	// ���⿡�� Min, Max ������ ���õǰ�, SphereInfo Center, Radius ������ ���õȴ�.
 	m_Mesh->SetMesh("PlayerMesh");
	m_Mesh->SetReceiveDecal(false);

	// Animation
	m_Mesh->CreateAnimationInstance<CPlayerAnimation>();
	m_Animation = (CPlayerAnimation*)m_Mesh->GetAnimationInstance();

	// Scale
	m_Mesh->SetRelativeScale(0.02f, 0.02f, 0.02f);

	m_Arm->SetOffset(0.f, 2.f, 0.f);
	m_Arm->SetRelativeRotation(25.f, 0.f, 0.f);
	m_Arm->SetTargetDistance(10.f);

	const Vector3& AnimComponentMeshSize = m_Mesh->GetMeshSize();
	const Vector3& MeshRelativeScale = m_Mesh->GetRelativeScale();

	// �Ʒ� ��ġ�� ���� (������ ���� ũ��� �̿� �°� ��������� �Ѵ�)
	// Vector3 ColliderLength = Vector3(
	// 	AnimComponentMeshSize.x * MeshRelativeScale.x,
	// 	AnimComponentMeshSize.y * MeshRelativeScale.y,
	// 	AnimComponentMeshSize.z * MeshRelativeScale.z
	// );

	Vector3 ColliderLength = Vector3(
		AnimComponentMeshSize.x * MeshRelativeScale.x * 0.5f, // ������ �������, ������ ������ ����� �Ѵ�.
		AnimComponentMeshSize.y * MeshRelativeScale.y,
		AnimComponentMeshSize.z * MeshRelativeScale.z
	);

	// Collider
	m_ColliderBox3D = CreateComponent<CColliderBox3D>("ColliderBox3D");
	m_ColliderBox3D->SetCollisionProfile("Player");
	m_Mesh->AddChild(m_ColliderBox3D);

	// Center ������ ���, �⺻������ Player �� WorldPos �� �߹����� ������.
	// ��, �ƹ� ó���� ������ ���� ���, Center �� �߹����� �����ٴ� �ǹ��̴�.
	// MeshSize y��ŭ 0.5 �÷��� Center �� ���� ���̴�.
	// �ش� ���� ������ �̿��ص� �ȴ�.
	m_ColliderBox3D->SetInfo(m_RootComponent->GetSphereOriginInfo().Center, ColliderLength * 0.5f);

	/*
	m_ColliderSphere = CreateComponent<CColliderSphere>("ColliderSphere");
	m_Mesh->AddChild(m_ColliderSphere);
	m_ColliderSphere->SetCollisionProfile("Player");
	const Vector3& AnimComponentMeshSize = m_Mesh->GetMeshSize();
	const Vector3& MeshRelativeScale = m_Mesh->GetRelativeScale();
	Vector3 ColliderCenter = Vector3(
		GetWorldPos().x,
		GetWorldPos().y + AnimComponentMeshSize.y * MeshRelativeScale.y * 0.5f,
		GetWorldPos().z
	);

	float ColliderRadiius = AnimComponentMeshSize.x * MeshRelativeScale.x;
	ColliderRadiius = AnimComponentMeshSize.y * MeshRelativeScale.y < ColliderRadiius ?
		AnimComponentMeshSize.y * MeshRelativeScale.y : ColliderRadiius;
	ColliderRadiius = AnimComponentMeshSize.z * MeshRelativeScale.z < ColliderRadiius ?
		AnimComponentMeshSize.z * MeshRelativeScale.z : ColliderRadiius;

	m_ColliderSphere->SetInfo(ColliderCenter, ColliderRadiius);
	*/

	// Picking �� Box
	m_CullingArea3D = CreateComponent<CPickingLayerBox3D>("ColliderBox3D");
	m_Mesh->AddChild(m_CullingArea3D);
	m_CullingArea3D->SetLength(ColliderLength * 0.5f);

	// Weapon ���� ����
	m_Weapon = m_Scene->CreateGameObject<CWeapon>("Weapon");
	m_Mesh->AddChild(m_Weapon, "Weapon");

	CInput::GetInst()->SetKeyCallback<CPlayer>("MoveFront", KeyState_Push,
		this, &CPlayer::MoveFront);
	CInput::GetInst()->SetKeyCallback<CPlayer>("MoveBack", KeyState_Push,
		this, &CPlayer::MoveBack);
	CInput::GetInst()->SetKeyCallback<CPlayer>("RotationYInv", KeyState_Push,
		this, &CPlayer::RotationYInv);
	CInput::GetInst()->SetKeyCallback<CPlayer>("RotationY", KeyState_Push,
		this, &CPlayer::RotationY);

	// 3���� Nav Mesh
	// CInput::GetInst()->SetKeyCallback<CPlayer>("MovePoint", KeyState_Down,
	// 	this, &CPlayer::MovePoint);

	CInput::GetInst()->SetKeyCallback<CPlayer>("Attack1", KeyState_Down,
		this, &CPlayer::Attack);

	return true;
}

void CPlayer::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);

	if (CInput::GetInst()->GetWheelDir())
	{
		float Length = m_Arm->GetTargetDistance() +
			CInput::GetInst()->GetWheelDir() * 1.0f;

		m_Arm->SetTargetDistance(Length);
	}

	//m_Arm->AddRelativeRotationY(90.f * DeltaTime);
	if (m_Velocity.Length() > 0.f)
	{
		//m_Animation->ChangeAnimation("Walk");
		m_Animation->SetIdleEnable(true);
	}

	else if(m_Animation->GetIdleEnable())
	{
		m_Animation->ChangeAnimation("Idle");
	}

	CGameObject* PickObj = nullptr;

	bool PickResult = m_Scene->Picking(PickObj);

	if (PickResult)
	{
		// Picking ����� LandScape ���, DDT �˰����� �̿��� �̵� ó���� �����Ѵ�.
		// if (PickObj->GetRootComponent()->GetTypeID() != typeid(CLandScape).hash_code())
		// 	return;

		if (CInput::GetInst()->GetMouseLButtonClick())
		{
			// ������ Ŭ���� �Ǿ��ٸ� �ش� ��ġ�� �̵���Ų��.
			// m_Scene->DDTPicking(PickObj, this);
			bool CheckResult = false;
		}
	}
}

void CPlayer::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);

	//Vector3	Pos = GetWorldPos();

	//Pos.y = m_Scene->GetNavigation3DManager()->GetY(Pos);

	//SetWorldPos(Pos);

	m_Velocity = Vector3::Zero;
}

CPlayer* CPlayer::Clone()
{
	return new CPlayer(*this);
}

void CPlayer::MoveFront(float DeltaTime)
{
	m_Velocity += GetWorldAxis(AXIS_Z) * 10.f * DeltaTime;

	AddWorldPos(GetWorldAxis(AXIS_Z) * 10.f * DeltaTime);
}

void CPlayer::MoveBack(float DeltaTime)
{
	m_Velocity += GetWorldAxis(AXIS_Z) * -10.f * DeltaTime;

	AddWorldPos(GetWorldAxis(AXIS_Z) * -10.f * DeltaTime);
}

void CPlayer::RotationYInv(float DeltaTime)
{
	AddWorldRotationY(-180.f * DeltaTime);
}

void CPlayer::RotationY(float DeltaTime)
{
	AddWorldRotationY(180.f * DeltaTime);
}

void CPlayer::Attack(float DeltaTime)
{
	m_Animation->ChangeAnimation("Attack");
	m_Animation->SetIdleEnable(false);
}

void CPlayer::MovePoint(float DeltaTime)
{
	Vector3 Point = m_Scene->GetNavigation3DManager()->GetPickingPos();

	Move(Point);
}
