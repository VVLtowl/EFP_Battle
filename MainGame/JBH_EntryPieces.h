#pragma once
#include "JudgementBehaviour.h"
#include <list>

class JBH_EntryPieces :
	public JudgementBehaviour
{
private:
	std::list<Player*>::iterator m_PlayerEnd;
	std::list<Player*>::iterator m_PlayerIterator;
	int m_AniOverCount;
	int m_PlayerAmount;

	int m_EntryFrameCount;
	int m_EntryDuration;

public:
	void Start()override;
	void MainUpdate()override;
	void End()override;
	int ID() override { return BHID_JUD_ENTRY_PIECES; };
};