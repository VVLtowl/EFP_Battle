#include "main.h"
#include "GameObject.h"
#include "Component.h"


Component::Component(GameObject* owner, int order):
	m_Owner(owner),
	m_UpdateOrder(order),
	m_State(ACTIVE)
{
	if (m_Owner != nullptr)
	{
		m_Owner->AddComponent(this);
	}
}

Component::~Component()
{
	if (m_Owner != nullptr)
	{
		m_Owner->RemoveComponent(this);
	}
}

void Component::Update()
{
}
