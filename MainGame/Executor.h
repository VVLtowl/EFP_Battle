#pragma once
#include <list>
#include <string>

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
	void Stop();
	void JumpToBH(Behaviour* bh);
	void StartBH(Behaviour* bh);
	Behaviour* GetBH() { return m_ExecuteBehaviour; };
	virtual std::string Name() = 0;

public:
	void SetExecuteBehaviour(Behaviour* bh) { m_ExecuteBehaviour = bh; };
};
