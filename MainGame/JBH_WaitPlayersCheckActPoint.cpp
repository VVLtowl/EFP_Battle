#include "main.h"
#include "JBH_WaitPlayersCheckActPoint.h"

#include "Judgement.h"

void JBH_WaitPlayersCheckActPoint::Start()
{
	m_PlayerEnd = GameManager::Instance()->Players.end();
	m_PlayerIterator = GameManager::Instance()->Players.begin();
}

void JBH_WaitPlayersCheckActPoint::MainUpdate()
{
	if (m_PlayerIterator == m_PlayerEnd)
	{
		NextState();
	}
	else
	{
		Player* player = (*m_PlayerIterator);
		Behaviour* checkActpoint = player->PieceData.OwnCharacter->FixedBHs[Character::CHECK_ACTPOINT];
		m_Excuter->JumpTo(checkActpoint);

		m_PlayerIterator++;
	}
}
