#include "Behaviour.h"
#include "Executor.h"


Executor::Executor()
{
	GameManager::Register(this);
}

Executor::~Executor()
{
	while (m_Behaviours.empty()==false)
	{
		delete m_Behaviours.back();
	}

	GameManager::Quit(this);
}


void Executor::Execute()
{
	if (m_ExecuteBehaviour != nullptr)
	{
		m_ExecuteBehaviour->MainUpdate();
	}
};


void Executor::Stop()
{
	m_ExecuteBehaviour = nullptr;
}

void Executor::JumpToBH(Behaviour* bh)
{
	if (bh)
	{
		bh->m_Next = m_ExecuteBehaviour;
		m_ExecuteBehaviour = bh;
	}
}

void Executor::StartBH(Behaviour* bh)
{
	//set execute behaviour and reset
	m_ExecuteBehaviour = bh;
	if (m_ExecuteBehaviour)
	{
		m_ExecuteBehaviour->ResetState();
	}
}
;