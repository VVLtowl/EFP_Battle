#pragma once
#include "Component.h"

enum ColliderType
{
	COL_LINE,
	COL_SPHERE,
	COL_AABB,

	COL_COUNT,
};

class Collider :
    public Component
{
public:
    Collider(class GameObject* owner,
        int order = COMP_COUNT);
    virtual ~Collider();

protected:
    int m_ColliderType;

public:
    int GetColliderType()const { return m_ColliderType; };
};

