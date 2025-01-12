#pragma once

#include "GameObject\GameObject.h"
#include "Component/DecalComponent.h"

class CDecalObj :
    public CGameObject
{
    friend class CScene;

protected:
    CDecalObj();
    CDecalObj(const CDecalObj& obj);
    virtual ~CDecalObj();

private:
    CSharedPtr<CDecalComponent>    m_Decal;

    // 원래 Culling 및 Picking 용은 "Sphere"로 판단하는 중이다.
// 단, 우선적으로 debugging을 위해 파란색으로 SphereInfo로 판단되는 지점을 세팅할 것이다. (범위는 min, max)
    CSharedPtr<class CPickingLayerBox3D>        m_CullingArea3D;
public:
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CDecalObj* Clone();
};

