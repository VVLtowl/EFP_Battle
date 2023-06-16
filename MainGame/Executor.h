#pragma once
#include <list>
#include <string>

class Behaviour;

class Executor
{
	friend Behaviour;

	/*********************************************************
	* @brief	executor's behaviour
	********************************************************/
protected:
	// @brief	execute currently 
	Behaviour* m_ExecuteBehaviour = nullptr;
	// @brief	behaviour list
	std::list<Behaviour*> m_Behaviours;

	/*********************************************************
	* @brief	executor method
	********************************************************/
public:
	Executor();
	virtual ~Executor();

	// @brief	use in main update
	void Execute();
	void Stop();

	// @brief	A jump to B,
	//			when B finished, back to A 
	void JumpToBH(Behaviour* bh);
	// @brief	stop A and execute B 
	void StartBH(Behaviour* bh);
	// @brief	get executing behaviour 
	Behaviour* GetBH() { return m_ExecuteBehaviour; };
	virtual std::string Name() = 0;

	// @brief	launch
	void SetExecuteBehaviour(Behaviour* bh) { m_ExecuteBehaviour = bh; };
};
