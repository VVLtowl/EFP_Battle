#include "main.h"
#include "debugInfo.h"
#include "Executor.h"
#include "Behaviour.h"


void Behaviour::Print(std::string info)
{
	if (m_PrintRowID == -1)
	{
		m_PrintRowID = DebugInfo::Print(
			"[" +
			info +
			"] BH-" +
			Name());
	}
	else
	{
		DebugInfo::PrintRow(m_PrintRowID,
			"[" +
			info +
			"] BH-" +
			Name());
	}
}

void Behaviour::SwitchToNext(Behaviour* bh)
{
	m_State = END;
	m_Next = bh;
}

void Behaviour::JumpTo(Behaviour* jump)
{ 
	m_Executor->JumpToBH(jump); 
}

Behaviour::Behaviour(Executor* executor, bool isOnce)
{
	SetExecutor(executor);
	
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
	if (m_State == START)
	{
		m_PrintRowID = -1;
		Print("start");
		Start();
		NextState();
	}

	if (m_State == UPDATE)
	{
		//need NextState() in Update()
		Print("update");
		Update();
	}
	
	if (m_State == END)
	{
		Print("finish");

		//when m_Next is nullptr, executor do nothing next
		m_Executor->SetExecuteBehaviour(m_Next);

		//from this time, cant set next behaviour
		m_EndEvent();
		End();

		if (m_IsOnceBehaviour)
			//delete this should be at last
		{
			delete this;
		}
		else
		{
			//todo
			//tips: BH_WaitPlayersFinish is constant, need to clear nextBH after set to executor's BH
			SetNext(nullptr);
			ResetState();
		}
	}
}

