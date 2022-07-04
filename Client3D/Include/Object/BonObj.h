#pragma once

#include "GameObject\GameObject.h"
#include "Component/StaticMeshComponent.h"

class CBonObj :
    public CGameObject
{
    friend class CScene;

protected:
    CBonObj();
    CBonObj(const CBonObj& obj);
    virtual ~CBonObj();

private:
    CSharedPtr<CStaticMeshComponent>    m_Mesh;

    // ���� Culling �� Picking ���� "Sphere"�� �Ǵ��ϴ� ���̴�.
// ��, �켱������ debugging�� ���� �Ķ������� SphereInfo�� �ǴܵǴ� ������ ������ ���̴�. (������ min, max)
    CSharedPtr<class CPickingLayerBox3D>        m_CullingArea3D;
public:
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CBonObj* Clone();
};

