#pragma once
#include "Component.h"

class LookAtCamera:
    public Component
{
public:
    LookAtCamera(class GameObject* owner,
        class Camera* cmr,
        int order = COMP_LOOKATVIEW);
    ~LookAtCamera();
    void Update()override;

public:
    class Camera* TargetCamera;
};

class LookAtCameraMatrix
{
public:
    D3DXMATRIX operator()(
        const D3DXVECTOR3& pos,
        const D3DXVECTOR3& scl, 
        class Camera* cmr)const;
};

