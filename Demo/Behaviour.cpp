#include "Behaviour.h"
#include <iostream>


void Event::Excute()
{
	cout << "event" << endl;
}

void Behaviour::AddStartEvent(Event* ev)
{
	m_StartEvent.push(ev);
}

void Behaviour::AddEndEvent(Event* ev)
{
	m_EndEvent.push(ev);
}

void Behaviour::SetNext(Behaviour* next)
{
	m_Next = next;
}

void Behaviour::Update()
{
	if (m_State == START)
	{
		while (!m_StartEvent.empty())
		{
			m_StartEvent.top()->Excute();
			m_StartEvent.pop();
		}
		MainStart();
		//cout << " start " << endl;
		//break;
	}
	if (m_State == UPDATE)
	{
		Update();
		//cout << " update " << endl;
		//break;
	}
	if (m_State == END)
	{
		while (!m_EndEvent.empty())
		{
			m_EndEvent.top()->Excute();
			m_EndEvent.pop();
		}
		MainEnd();
		//cout << " end " << endl;
		//break;
	}
	if (m_State == NEXT)
	{
		if (m_Next != nullptr)
		{
			Game::Instance.SetExcuteBH(m_Next);
		}
		NextState();
		//cout << " next frame finish" << endl;
		//break;
	}
}

void Behaviour::MainStart()
{
	NextState();
}
void Behaviour::Update()
{
	NextState();
}
void Behaviour::MainEnd()
{
	NextState();
}

void Behaviour::NextState()
{
	m_State = (State)((m_State + 1) % MAX);
}

void Behaviour::Print(string info)
{
	cout << "behaviour " << m_ID << ": " << info << endl;
}

void JBH_TurnStart::MainStart()
{
	m_Player = Game::Instance.m_Players.begin();

	NextState();
}

void JBH_TurnStart::Update()
{
	Print("step: Turn StartBH");

#if 0
	for (auto player : Game::Instance.m_Players)
	{
		player->GetFixedBehaviour(Player::FBH_CLEAR_POINT)->Excute();
	}
#endif

#if 1
	if (m_Player!= Game::Instance.m_Players.end())
	{
		Player* player = *(m_Player);
		Behaviour* bh = player->GetFixedBehaviour(Player::FBH_CLEAR_POINT);
		Game::Instance.JumpToBH(bh, this);
		
		m_Player++;
		return;
	}
#endif

	NextState();
}

void JBH_RPS::MainStart()
{
	m_Player = Game::Instance.m_Players.begin();

	NextState();
}

void JBH_RPS::Update()
{
	Print("step: RSP");

#if 0
	for (auto player : Game::Instance.m_Players)
	{
		player->GetFixedBehaviour(Player::FBH_INPUT_HAND)->Excute();
	}
#endif


#if 1
	if (m_Player != Game::Instance.m_Players.end())
	{
		Player* player = *(m_Player);
		Behaviour* bh = player->GetFixedBehaviour(Player::FBH_INPUT_HAND);
		Game::Instance.JumpToBH(bh, this);

		m_Player++;
		return;
	}
#endif

	NextState();
}

void JBH_CalculateActPoint::Update()
{
	Print("step: Calculate Act Point");

	//set up camp
	list<Player*> badCamp;
	list<Player*> goodCamp;
	for (auto player : Game::Instance.m_Players)
	{
		if (player->m_Camp == Character::CAMP_BAD)
		{
			badCamp.push_back(player);
		}
		else if (player->m_Camp == Character::CAMP_GOOD)
		{
			goodCamp.push_back(player);
		}
	}

	//normal camp battle
	for (auto bad : badCamp)
	{
		for (auto good : goodCamp)
		{
			for (auto badHand : bad->m_Hands)
			{
				for (auto goodHand : good->m_Hands)
				{
					RPS::ResultType badRst = RPS::Battle((Hand::HandType)badHand, (Hand::HandType)goodHand);
					RPS::ResultType goodRst = RPS::Battle((Hand::HandType)goodHand, (Hand::HandType)badHand);
					if (badRst == RPS::RESULT_WIN)
					{
						bad->m_ActPoint++;
					}
					if (goodRst == RPS::RESULT_WIN)
					{
						good->m_ActPoint++;
					}

					//test
					cout << bad->m_Name << "[" << Hand::HandName((Hand::HandType)badHand) << "] : "
						<< good->m_Name << "[" << Hand::HandName((Hand::HandType)goodHand) << "]" << endl
						<< badRst << " : " << goodRst << endl;
				}
			}
		}
	}

	//special battle

	//test
	for (auto p : Game::Instance.m_Players)
	{
		cout << p->m_Name << " act point : " << p->m_ActPoint << endl;
	}

	NextState();
}

void JBH_GroupAct::Update()
{
	Print("step: Group Act");

	bool hasCanActPlayer = true;
	m_Player = m_ActGroup->Next();
	while (m_Player->m_ActPoint == 0)
	{
		m_ActOverCount++;
		if (m_ActOverCount < m_ActMax)
		{
			m_Player = m_ActGroup->Next();
		}
		else
		{
			hasCanActPlayer = false;
			break;
		}
	}
	if (hasCanActPlayer == true)
	{
		Behaviour* bh = m_Player->GetFixedBehaviour(Player::FBH_SELECT_ACT);
		Game::Instance.JumpToBH(bh, this);

		return;
	}

	NextState();
}

void JBH_BadManAct::MainStart()
{
	m_ActOverCount = 0;
	m_ActGroup = &(m_OwnerJudgement->m_BadActGroup);
	m_ActMax = m_ActGroup->m_List.size();

	m_Player = m_ActGroup->First();

	NextState();
}


void JBH_GoodManAct::MainStart()
{
	m_ActOverCount = 0;
	m_ActGroup = &(m_OwnerJudgement->m_GoodActGroup);
	m_ActMax = m_ActGroup->m_List.size();

	m_Player = m_ActGroup->First();

	NextState();
}

void JBH_TurnEnd::Update()
{
	Print("step: Turn End");

	getchar();
	NextState();
}















void PieceBehaviour::SetPiece(Player* player)
{
	m_Player = player;
}

void PBH_ClearPoint::Update()
{
	Print("player: Clear Point");

	m_Player->m_ActPoint = 0;

	NextState();
}

void PBH_InputHand::Update()
{
	Print("player: Input Hand");

	//select hand


	//comfirm hand
	//test
	int hand = rand() % 3 + 1;
	m_Player->m_Hands.clear();
	m_Player->m_Hands.push_back(hand);

	NextState();
}

void PBH_SelectAct::MainStart()
{
	m_SelectedAct = m_Player->GetActBehaviourList()->begin();

	m_State = UPDATE;
}

void PBH_SelectAct::Update()
{
	Print("player: Select Act");

	//select act
	

	//jump to act
	if (m_Player->m_ActPoint > 0)
	{		
		Game::Instance.JumpToBH((*m_SelectedAct), this);
		return;
	}

	NextState();
}

void PBH_Move::Update()
{
	Print("player : Move Piece");

	//test
	m_Player->m_ActPoint--;

	NextState();
}

void PBH_UpdateState::Update()
{
	Print("player : Update State");

	NextState();
}

