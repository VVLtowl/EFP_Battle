#pragma once
#include "Component.h"

class WorldPositionToScreenPosition
{
public:
    D3DXVECTOR2& operator()(const D3DXVECTOR3& worldPos)const;
};


class UIFollowWorldObject :
    public Component
{
public:
    UIFollowWorldObject(
        class GameObject* owner,
        class Transform3D* followTarget = nullptr,
        int order = COMP_WORLDTOSCREEN);
    ~UIFollowWorldObject();
    void Update()override;

private:
    class Transform3D* TargetTransform;
public:
    void SetTargetTransform(class Transform3D* target) { TargetTransform = target; };
};

