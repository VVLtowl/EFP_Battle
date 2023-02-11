#pragma once
#include "JudgementBehaviour.h"
#include "ShowUIHelper.h"

class JBH_ShowTurnStart :
	public JudgementBehaviour
{
private:
	class UITurnStart* m_TurnStart;
	ShowUIHelper m_ShowUI;

public:
	JBH_ShowTurnStart();
	~JBH_ShowTurnStart();

	void Start()override;
	void MainUpdate()override;
	int ID() override { return BHID_JUD_SHOW_TURN_START; };
};

