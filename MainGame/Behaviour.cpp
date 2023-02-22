#include "main.h"
#include "debugInfo.h"
#include "Executor.h"
#include "Behaviour.h"


void Behaviour::PrintState(std::string info)
{
	SetStateName(info);

	std::string str =
		"[" +
		info +
		"] BH-" +
		Name() +
		(m_Next ? "(next[" + m_Next->Name() + "])" : "") +
		(HasEndEvent() ? "(end event[" + GetEndEventInfo() + "])" : "");

	DebugInfo::Print(str);
#if 0
	if (m_PrintRowID == -1)
	{
		m_PrintRowID = DebugInfo::Print(str);
	}
	else
	{
		DebugInfo::PrintRow(m_PrintRowID, str);
	}
#endif
}

void Behaviour::PrintUpdateState(std::string info)
{
	SetStateName(info);

	std::string str =
		"[" +
		info +
		"] BH-" +
		Name() +
		(m_Next ? "(next[" + m_Next->Name() + "])" : "") +
		(HasEndEvent() ? "(end event[" + GetEndEventInfo() + "])" : "");

	//when different state info, reset row id
	if (info != m_StateName)
	{
		m_PrintRowID = -1;
	}

	//check if need new print on another row
	if (m_PrintRowID == -1)
	{
		m_PrintRowID = DebugInfo::Print(str);
	}
	else
	{
		DebugInfo::PrintRow(m_PrintRowID, str);
	}
}

//void Behaviour::SwitchToNext(Behaviour* bh)
//{
//	m_State = State::END;
//	m_Next = bh;
//}
//
//void Behaviour::JumpTo(Behaviour* jump)
//{ 
//	m_Executor->JumpToBH(jump); 
//}

Behaviour::Behaviour(Executor* executor, bool isOnce):
	m_Executor(executor)
{
	//SetExecutor(executor);
	
	//old version
	//BehaviourFactory::Register(this);
	//manage by executor
	executor->m_Behaviours.emplace_back(this);
	
	m_EndEvent = []() {; };
	m_IsOnceBehaviour = isOnce;
}

Behaviour::~Behaviour()
{
	//old version
	//BehaviourFactory::Quit(this);
	//manage by executor
	m_Executor->m_Behaviours.remove(this);
}

void Behaviour::MainUpdate()
{
	//start and update in same frame
	if (m_State == State::START)
	{
		m_PrintRowID = -1;
		PrintState("start");
		Start();
		NextState();
	}

	if (m_State == State::UPDATE)
	{
		//need NextState() in Update()
		PrintUpdateState("update"); 
		Update();
	}
	
	if (m_State == State::END)
	{
		PrintState("end");

		//when m_Next is nullptr, executor do nothing next
		m_Executor->SetExecuteBehaviour(m_Next);
		//tips: BH_WaitPlayersFinish is constant, need to clear nextBH after set to executor's BH
		m_Next = nullptr;

		//reset state
		ResetState();

		//from this time, cant set next behaviour
		m_EndEvent();

		//delete this should be at last
		if (m_IsOnceBehaviour)
		{
			delete this;
		}
	}
}

