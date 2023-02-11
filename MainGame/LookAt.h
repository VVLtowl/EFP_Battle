#pragma once
#include "Component.h"

class LookAt :
    public Component
{
public:
    LookAt(class GameObject* owner,
        class Transform3D* target=nullptr,
        int order= COMP_LOOKATTRANSFORM);
    ~LookAt();
    void Update()override;

public:
    class Transform3D* TargetTransform;
};

