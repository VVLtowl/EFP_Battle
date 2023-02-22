#pragma once

class StateOwner;
class State;

class StateOwner
{
protected:
	State* m_State;
public:
	void SetState(State* state) { m_State = state; };
	~StateOwner();
};

class State
{
public:
	StateOwner* m_Owner;

public:
	State(StateOwner* owner) :m_Owner(owner) { m_Owner->SetState(this); };
	virtual ~State() {};
};

inline StateOwner::~StateOwner()
{
	if (m_State) 
	{ 
		delete m_State; 
	}
};
