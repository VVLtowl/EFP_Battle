#pragma once
#include "Singleton.h"
#include "State.h"

class StateFactory :
	public Singleton<StateFactory>
{
public:
	template <typename ConcreteStateType>
	ConcreteStateType* Create(StateOwner* owner);
};

template<typename ConcreteStateType>
inline ConcreteStateType* StateFactory::Create(StateOwner* owner)
{
	ConcreteStateType* state = new ConcreteStateType();
	state->Owner = owner;
	owner->NowState = state;

	return state;
}
