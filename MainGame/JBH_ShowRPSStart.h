#pragma once
#include "JudgementBehaviour.h"
#include "ShowUIHelper.h"

class JBH_ShowRPSStart :
    public JudgementBehaviour
{
private:
	ShowUIHelper m_ShowUI;

public:
	void Start()override;
	void MainUpdate()override;
	void End()override;
	int ID() override { return BHID_JUD_SHOW_RPS_START; };
};

