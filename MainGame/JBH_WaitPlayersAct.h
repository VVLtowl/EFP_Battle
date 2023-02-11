#pragma once
#include "JudgementBehaviour.h"

class JBH_WaitPlayersAct :
    public JudgementBehaviour
{
public:
    //init
    void SetIsBad(bool bad);

public:
    void Start()override;
    void MainUpdate()override;
    void End()override;
    int ID() override { return m_BHID; };


private:
    bool m_IsBad;
    class LoopList<class Player*>* m_PlayerLoopList;
    class Player* m_NowPlayer;
    int m_BHID;

    int m_FinishCount;
    int m_PlayerAmount;
};

