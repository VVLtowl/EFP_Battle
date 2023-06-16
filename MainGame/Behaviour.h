#pragma once
#include <stack>
#include <string>
#include <functional>
#include "BehaviourFactory.h"

class Executor;

class Behaviour
{
	friend Executor;

	/*********************************************************
	* @brief	behaviour state
	********************************************************/
public:
	enum class State
	{
		START,
		UPDATE,
		END,
	};
	std::string GetStateName() { return m_StateName; };
	State GetState() const { return m_State; };
protected:
	void NextState() { m_State = (State)((int)m_State + 1); };
	void ResetState() { m_State = State::START; };
private:
	State m_State = State::START;

	/*********************************************************
	* @brief	behaviour method and value
	********************************************************/
public:
	Behaviour(class Executor* executor, bool isOnce = false);
	virtual ~Behaviour();

	// @brief	use in main update 
	void MainUpdate();
	// @brief	custormize reset, 
	//			NotOnceBehaviour need reset before using
	virtual void Reset() { ResetState(); };
	// @brief	for check
	virtual std::string Name() { return "no name behaviour"; };
protected:
	// @brief	custormize start
	virtual void Start() {};
	// @brief	custormize update
	virtual void Update() { NextState(); };
protected:
	// @brief	owner
	Executor* m_Executor = nullptr;
	// @brief	OnceBehaviour: delete itself after finishing
	bool m_IsOnceBehaviour = false;


	/*********************************************************
	* @brief	next behaviour
	********************************************************/
public:
	Behaviour* GetNext() { return m_Next; };
protected:
	Behaviour* m_Next = nullptr;


	/*********************************************************
	* @brief	end event
	********************************************************/
public:
	// @brief	custormize end event
	void SetEndEvent(std::string info, std::function<void()> func = []() {; })
	{
		m_EndEventInfo = info;
		m_EndEvent = func;
	};
	bool HasEndEvent() { return m_EndEventInfo != ""; };
	// @brief	for check end event info
	std::string GetEndEventInfo() { return m_EndEventInfo; };
protected:
	std::function<void()> m_EndEvent;
	std::string m_EndEventInfo = "";




	/*********************************************************
	* @brief	check info
	********************************************************/
protected:
	void PrintState(std::string info);
	void PrintUpdateState(std::string info);
	void SetStateName(std::string stateName) { m_StateName = stateName; };
protected:
	std::string m_StateName = "";
	int m_PrintRowID = -1;//for print info on same row
};

