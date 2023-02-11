#pragma once
#include "JudgementBehaviour.h"
#include "ShowUIHelper.h"

class JBH_ShowGameStart :
	public JudgementBehaviour
{
private:
	ShowUIHelper m_ShowUI;

public:
	void Start()override;
	void MainUpdate()override;
	void End()override;
	int ID() override { return BHID_JUD_SHOW_GAME_START; };
};