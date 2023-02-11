#pragma once
#include "JudgementBehaviour.h"

class JBH_WaitPlayersRPS :
    public JudgementBehaviour
{
public:
	enum State
	{
		START_PLAYER_RPS,
		PLAYER_RPS_FINISH,
	};
public:
	int WaitState;


public:
	void Start()override;
	void MainUpdate()override;
	int ID() override { return BHID_JUD_WAIT_PLAYERS_RPS; };

private:
	std::list<Player*>::iterator m_PlayerEnd;
	std::list<Player*>::iterator m_PlayerIterator;
};

