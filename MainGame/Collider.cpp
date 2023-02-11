#include "main.h"
#include "Collider.h"

Collider::Collider(GameObject* owner, int order):
	Component(owner,order),
	m_ColliderType(COL_COUNT)
{
	ColliderManager::Register(this);
}

Collider::~Collider()
{
	ColliderManager::Quit(this);
}
