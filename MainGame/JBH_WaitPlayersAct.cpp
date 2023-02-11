#include "main.h"
#include "JBH_WaitPlayersAct.h"

#include "Judgement.h"

void JBH_WaitPlayersAct::SetIsBad(bool bad)
{
	m_IsBad = bad;
	if (m_IsBad)
	{
		m_BHID = BHID_JUD_WAIT_BAD_PLAYERS_ACT;
	}
	else
	{
		m_BHID = BHID_JUD_WAIT_GOOD_PLAYERS_ACT;
	}
}

void JBH_WaitPlayersAct::Start()
{
	//reset loop list by isbad
	{
		if (m_IsBad)
		{
			m_PlayerLoopList = &m_Judgement->BadActGroup;
		}
		else
		{
			m_PlayerLoopList = &m_Judgement->GoodActGroup;
		}
		m_NowPlayer = m_PlayerLoopList->First();
	}

	//set finish count
	{
		m_FinishCount = 0;
		m_PlayerAmount = m_PlayerLoopList->GetList()->size();
	}
}

void JBH_WaitPlayersAct::MainUpdate()
{
	if (m_FinishCount == m_PlayerAmount)
	{
		NextState();
	}
	else
	{
	/*	Player* player = (*m_PlayerIterator);
		Behaviour* checkActpoint = player->OwnCharacter->FixedBHs[Character::CHECK_ACTPOINT];
		m_Excuter->JumpTo(checkActpoint);

		m_PlayerIterator++;*/
	}
}

void JBH_WaitPlayersAct::End()
{
}
