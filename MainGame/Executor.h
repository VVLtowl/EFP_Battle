#pragma once
#include <list>

class Behaviour;

class Executor
{
	friend Behaviour;
protected:
	Behaviour* m_ExecuteBehaviour = nullptr;
	std::list<Behaviour*> m_Behaviours;

public:
	Executor();
	virtual ~Executor();

	void Execute();
	void JumpToBH(Behaviour* bh);
	void StartBH(Behaviour* bh);

public:
	void SetExecuteBehaviour(Behaviour* bh) { m_ExecuteBehaviour = bh; };
};
