#pragma once
#include "JudgementBehaviour.h"
#include "ShowUIHelper.h"

class JBH_ShowActStart :
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
    class UIActStart* m_UI;
    ShowUIHelper m_ShowUI;
    int m_BHID;
    bool m_IsBad;
};