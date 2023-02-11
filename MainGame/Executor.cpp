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

void Executor::JumpToBH(Behaviour* bh)
{
	bh->SetNext(m_ExecuteBehaviour);
	m_ExecuteBehaviour = bh;
}

void Executor::StartBH(Behaviour* bh)
{
	if (m_ExecuteBehaviour != nullptr)
	{
		m_ExecuteBehaviour->SwitchToNext(bh);
	}
	else
	{
		m_ExecuteBehaviour = bh;
	}
}
;