#pragma once
#include "JudgementBehaviour.h"

class JBH_ShowPlayersHand :
    public JudgementBehaviour
{
public:
    void Start()override;
    void MainUpdate()override;
    int ID() override { return BHID_JUD_SHOW_PLAYERS_HAND; };

private:
    std::list<Player*>::iterator m_PlayerEnd;
    std::list<Player*>::iterator m_PlayerIterator;

    int m_AniOverCount;
    int m_PlayerAmount;

    int m_ShowUIFrameCount;
    int m_ShowUIDuration;
};

