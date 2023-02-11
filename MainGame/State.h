#pragma once

class StateOwner;
class State;

class StateOwner
{
public:
	State* NowState;
};

class State
{
public:
	StateOwner* Owner;

public:
	virtual void Start() {};
	virtual void Update() {};
	virtual ~State() {};
};
