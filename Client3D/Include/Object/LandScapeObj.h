#pragma once

#include "GameObject\GameObject.h"
#include "Component/LandScape.h"

class CLandScapeObj :
    public CGameObject
{
    friend class CScene;

protected:
    CLandScapeObj();
    CLandScapeObj(const CLandScapeObj& obj);
    virtual ~CLandScapeObj();

private:
    CSharedPtr<CLandScape>    m_LandScape;

    // ���� Culling �� Picking ���� "Sphere"�� �Ǵ��ϴ� ���̴�.
// ��, �켱������ debugging�� ���� �Ķ������� SphereInfo�� �ǴܵǴ� ������ ������ ���̴�. (������ min, max)
    CSharedPtr<class CPickingLayerBox3D>        m_CullingArea3D;

public:
    virtual bool Init();
    virtual void Update(float DeltaTime);
    virtual void PostUpdate(float DeltaTime);
    virtual CLandScapeObj* Clone();
};

