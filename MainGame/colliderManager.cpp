#include "main.h"
#include "colliderManager.h"

std::list<Collider*> ColliderManager::Colliders;

void ColliderManager::Register(Collider* col)
{
	Colliders.emplace_back(col);
}

void ColliderManager::Quit(Collider* col)
{
	Colliders.remove(col);
}
