#pragma once
#include <list>
#include "Collider.h"

#include "BoxCollider.h"

class ColliderManager
{
public:
	static std::list<Collider*> Colliders;

public:
	static void Register(Collider* col);
	static void Quit(Collider* col);
};

