#pragma once
#include "JudgementBehaviour.h"

class JBH_WaitPlayersCheckActPoint :
    public JudgementBehaviour
{

public:
    void Start()override;
    void MainUpdate()override;
    int ID() override { return BHID_JUD_WAIT_PLAYERS_CHECK_ACTPOINT; };


private:
    std::list<Player*>::iterator m_PlayerEnd;
    std::list<Player*>::iterator m_PlayerIterator;
};

