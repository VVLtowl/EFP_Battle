#pragma once
#include "Component.h"

class Transform:
	public Component
{
public:
	Transform(class GameObject* owner, int updateOrder = COMP_COUNT);//�X�V����Ԓx����
	virtual ~Transform();
	virtual void Update() override = 0;
};