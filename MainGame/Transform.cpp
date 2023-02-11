#include "Transform.h"

Transform::Transform(GameObject* owner, int updateOrder):
	Component(owner,updateOrder)
{
}

Transform::~Transform()
{
}
