
#include <iostream>
#include "Behaviour.h"
#include "main.h"
#include <windows.h>

Game Game::Instance;
RPS::ResultType const RPS::Result[Hand::HAND_MAX][Hand::HAND_MAX]=
{					  //self hand
	{{RESULT_NONE},{RESULT_NONE},{RESULT_NONE},{RESULT_NONE},{RESULT_NONE},},//none
	{{RESULT_NONE},{RESULT_DRAW},{RESULT_LOSE},{RESULT_WIN},{RESULT_LOSE},},//rock
	{{RESULT_NONE},{RESULT_WIN},{RESULT_DRAW},{RESULT_LOSE},{RESULT_LOSE},},//paper
	{{RESULT_NONE},{RESULT_LOSE},{RESULT_WIN},{RESULT_DRAW},{RESULT_LOSE},},//scissors
	{{RESULT_NONE},{RESULT_WIN},{RESULT_WIN},{RESULT_WIN},{RESULT_WIN},},//king
};
string const Hand::m_HandName[Hand::HAND_MAX] =
{
{"  "},
{"rock"},
{"paper"},
{"scissors"},
{"king"},
};

int main()
{
	srand((unsigned int)time(NULL));

	Game::Instance.PrepareGame();

	while (1)
	{
		Sleep(10);
		Game::Instance.UpdateGame();
	}

	getchar();
	return 0;
}




Character::Character()
{
	m_FixedBHID[Player::FBH_CLEAR_POINT] = BHID_PLAYER_CLEAR_POINT;
	m_FixedBHID[Player::FBH_INPUT_HAND] = BHID_PLAYER_INPUT_HAND;
	m_FixedBHID[Player::FBH_SELECT_ACT] = BHID_PLAYER_SELECT_ACT;
	m_FixedBHID[Player::PLAYER_BH_UPDATE_STATE] = BHID_PLAYER_UPDATE_STATE;
	m_FixedBHID[Player::PLAYER_BH_OPEN_STATUS] = BHID_PLAYER_OPEN_STATUS;
	m_FixedBHID[Player::PLAYER_BH_CAUGHT] = BHID_PLAYER_CAUGHT;
	m_FixedBHID[Player::PLAYER_BH_GET_TOOL] = BHID_PLAYER_GET_TOOL;

	m_ActBHIDs.push_back(BHID_PLAYER_MOVE);
	m_ActBHIDs.push_back(BHID_PLAYER_GIVE_TOOL);
	m_ActBHIDs.push_back(BHID_PLAYER_USE_TOOL);
	m_ActBHIDs.push_back(BHID_PLAYER_THEN_ACT);
	m_ActBHIDs.push_back(BHID_PLAYER_END_ACT);
}

BadMan::BadMan()
{
	m_Camp = CAMP_BAD;

	m_ActBHIDs.push_back(BHID_PLAYER_STEAL_TOOL);
}

GoodMan::GoodMan()
{
	m_Camp = CAMP_GOOD;

	m_ActBHIDs.push_back(BHID_PLAYER_CATCH);
}



void Judgement::Prepare()
{
	PrepareMainCycle();
	PreparePlayer();
}

void Judgement::PrepareMainCycle()
{
	//create jbh and set main cycle
	m_MainCycle.push_back(BehaviourFactory::CreateJudBehaviour(BHID_STEP_TURN_START, this));
	m_MainCycle.push_back(BehaviourFactory::CreateJudBehaviour(BHID_STEP_RPS, this));
	m_MainCycle.push_back(BehaviourFactory::CreateJudBehaviour(BHID_STEP_CALCULATE_ACT_POINT, this));
	m_MainCycle.push_back(BehaviourFactory::CreateJudBehaviour(BHID_STEP_BAD_MAN_ACT, this));
	m_MainCycle.push_back(BehaviourFactory::CreateJudBehaviour(BHID_STEP_GOOD_MAN_ACT, this));
	m_MainCycle.push_back(BehaviourFactory::CreateJudBehaviour(BHID_STEP_TURN_END, this));

	//make loop chain
	m_MainCycle[STEP_TURN_START]->SetNext(m_MainCycle[STEP_RPS]);
	m_MainCycle[STEP_RPS]->SetNext(m_MainCycle[STEP_CALCULATE_POINT]);
	m_MainCycle[STEP_CALCULATE_POINT]->SetNext(m_MainCycle[STEP_BAD_ACT]);
	m_MainCycle[STEP_BAD_ACT]->SetNext(m_MainCycle[STEP_GOOD_ACT]);
	m_MainCycle[STEP_GOOD_ACT]->SetNext(m_MainCycle[STEP_TURN_END]);
	m_MainCycle[STEP_TURN_END]->SetNext(m_MainCycle[STEP_TURN_START]);
}

void Judgement::PreparePlayer()
{
	BadMan badMan;
	GoodMan goodMan;

	//set player camp and name
	int playerCount = 0;
	for (auto player : Game::Instance.m_Players)
	{
		playerCount++;
		if (playerCount <= 2)
			//make good man
		{
			player->m_Camp = goodMan.m_Camp;
			player->m_Name = "good" + to_string(playerCount);
		}
		else
			//make bad game
		{
			player->m_Camp = badMan.m_Camp;
			player->m_Name = "bad" + to_string(playerCount - 2);
		}
	}

	//set player behaviours
	for (auto player : Game::Instance.m_Players)
	{
		Character* chara = nullptr;
		if (player->m_Camp == Character::CAMP_GOOD)
		{
			chara = &goodMan;
		}
		else
		{
			chara = &badMan;
		}

		//set fixed behaviour
		int fixedMax = Player::FBH_MAX;
		for (int i = 0; i < fixedMax; i++)
		{
			BehaviourID bhid = chara->m_FixedBHID[i];
			PieceBehaviour* pbh =
				BehaviourFactory::CreatePlayerBhaviour(bhid, player);

			player->SetFixedBehaviour((Player::FixedBehaviourID)i, pbh);
		}

		//set act behaviour
		for (auto bhid : chara->m_ActBHIDs)
		{
			PieceBehaviour* pbh =
				BehaviourFactory::CreatePlayerBhaviour(bhid, player);

			player->AddActBehaviour(pbh);
		}

		//set special behaviour

	}

	//set act group
	for (auto player : Game::Instance.m_Players)
	{
		if (player->m_Camp == Character::CAMP_BAD)
		{
			m_BadActGroup.m_List.push_back(player);
		}
		else if (player->m_Camp == Character::CAMP_GOOD)
		{
			m_GoodActGroup.m_List.push_back(player);
		}
	}

	//set behaviour next (make chain)
#if 0
	for (auto player : Game::Instance.m_Players)
	{
		//input hand -> rps
		Behaviour* stepRPS = m_MainCycle[STEP_RPS];
		player->GetFixedBehaviour(Player::FBH_INPUT_HAND)->SetNext(stepRPS);

		//act -> select act
		//then, end -> bad/good act
		Behaviour* playerSelectAct = player->GetFixedBehaviour(Player::FBH_SELECT_ACT);
		Behaviour* stepBadAct = m_MainCycle[STEP_BAD_ACT];
		Behaviour* stepGoodAct = m_MainCycle[STEP_GOOD_ACT];
		for (auto act : *(player->GetActBehaviourList()))
		{
			if (act->m_ID == BHID_PLAYER_THEN_ACT ||
				act->m_ID == BHID_PLAYER_END_ACT)
			{
				if (player->m_Camp == Character::CAMP_GOOD)
				{
					act->SetNext(stepGoodAct);
				}
				else if (player->m_Camp == Character::CAMP_BAD)
				{
					act->SetNext(stepBadAct);
				}
			}
			else if (act->m_ID == BHID_PLAYER_CLEAR_POINT)
			{

			}
			else
			{
				act->SetNext(playerSelectAct);
			}
		}
	}
#endif
}

void Judgement::Start()
{
	Game::Instance.m_ExcuteBehaviour = m_MainCycle[STEP_TURN_START];
}



void Player::SetFixedBehaviour(FixedBehaviourID id, Behaviour* bh)
{
	m_FixedBehaviour[id] = bh;
}

void Player::AddActBehaviour(Behaviour* bh)
{
	m_ActBehaviour.push_back(bh);
}

void Player::AddSpecialBehaviour(BehaviourID id, Behaviour* bh)
{
	m_SpecialBehaviour.emplace(id, bh);
}

Behaviour* Player::GetFixedBehaviour(FixedBehaviourID id)
{
	return m_FixedBehaviour[id];
}

list<Behaviour*>* Player::GetActBehaviourList()
{
	return &m_ActBehaviour;
}

Behaviour* Player::GetSpecialBehaviour(BehaviourID id)
{
	return (*(m_SpecialBehaviour.find(id))).second;
}

void Game::PrepareGame()
{
	//player
	for (int i = 0; i < 6; i++)
	{
		Player* player = PlayerFactory::Create();
	}

	//judge
	Instance.m_Judgement.Prepare();
	Instance.m_Judgement.Start();
}

void Game::UpdateGame()
{
	m_ExcuteBehaviour->Update();
}

void Game::SetExcuteBH(Behaviour* bh)
{
	m_ExcuteBehaviour = bh;
}

void Game::JumpToBH(Behaviour* bh, Behaviour* back)
{
	bh->SetNext(back);
	SetExcuteBH(bh);
}









template <typename ConstractBehaviourType>
ConstractBehaviourType* BehaviourFactory::Create()
{
	ConstractBehaviourType* bh = new ConstractBehaviourType();
	Game::Instance.m_Behaviours.push_back(bh);
	return bh;
}

JudgementBehaviour* BehaviourFactory::CreateJudBehaviour(BehaviourID bhid, Judgement* jud)
{
	JudgementBehaviour* jbh = nullptr;
	switch (bhid)
	{
	case BHID_STEP_TURN_START:
		jbh = BehaviourFactory::Create<JBH_TurnStart>();
		break;
	case BHID_STEP_RPS:
		jbh = BehaviourFactory::Create<JBH_RPS>();
		break;
	case BHID_STEP_CALCULATE_ACT_POINT:
		jbh = BehaviourFactory::Create<JBH_CalculateActPoint>();
		break;
	case BHID_STEP_BAD_MAN_ACT:
		jbh = BehaviourFactory::Create<JBH_BadManAct>();
		break;
	case BHID_STEP_GOOD_MAN_ACT:
		jbh = BehaviourFactory::Create<JBH_GoodManAct>();
		break;
	case BHID_STEP_TURN_END:
		jbh = BehaviourFactory::Create<JBH_TurnEnd>();
		break;
	case BHID_STEP_TEST_INSERT:
		jbh = BehaviourFactory::Create<JBH_Insert>();
		break;
	default:
		break;
	}
	jbh->SetJudgement(jud);

	return jbh;
}

PieceBehaviour* BehaviourFactory::CreatePlayerBhaviour(BehaviourID bhid, Player* player)
{
	PieceBehaviour* pbh = nullptr;
	switch (bhid)
	{
	case BHID_PLAYER_CLEAR_POINT:
		pbh = BehaviourFactory::Create<PBH_ClearPoint>();
		break;
	case BHID_PLAYER_INPUT_HAND:
		pbh = BehaviourFactory::Create<PBH_InputHand>();
		break;
	case BHID_PLAYER_SELECT_ACT:
		pbh = BehaviourFactory::Create<PBH_SelectAct>();
		break;
	case BHID_PLAYER_UPDATE_STATE:
		pbh = BehaviourFactory::Create<PBH_UpdateState>();
		break;
	case BHID_PLAYER_OPEN_STATUS:
		pbh = BehaviourFactory::Create<PBH_OpenStatus>();
		break;
	case BHID_PLAYER_CAUGHT:
		pbh = BehaviourFactory::Create<PBH_Caught>();
		break;
	case BHID_PLAYER_GET_TOOL:
		pbh = BehaviourFactory::Create<PBH_GetTool>();
		break;

	case BHID_PLAYER_MOVE:
		pbh= BehaviourFactory::Create<PBH_Move>();
		break;
	case BHID_PLAYER_GIVE_TOOL:
		pbh = BehaviourFactory::Create<PBH_GiveTool>();
		break;
	case BHID_PLAYER_CATCH:
		pbh = BehaviourFactory::Create<PBH_Catch>();
		break;
	case BHID_PLAYER_STEAL_TOOL:
		pbh = BehaviourFactory::Create<PBH_StealTool>();
		break;
	case BHID_PLAYER_USE_TOOL:
		pbh = BehaviourFactory::Create<PBH_UseTool>();
		break;
	case BHID_PLAYER_THEN_ACT:
		pbh = BehaviourFactory::Create<PBH_ThenAct>();
		break;
	case BHID_PLAYER_END_ACT:
		pbh = BehaviourFactory::Create<PBH_EndAct>();
		break;

	default:
		break;
	}
	pbh->SetPiece(player);

	return pbh;
}

template <typename ConstractBehaviourType>
typename ConstractBehaviourType* CreateBehaviour<ConstractBehaviourType>::operator()()
{
	return BehaviourFactory::Create<ConstractBehaviourType>();
}

Player* PlayerFactory::Create()
{
	Player* player = new Player;
	Game::Instance.m_Players.push_back(player);
	return player;
}

RPS::ResultType RPS::Battle( Hand::HandType main,  Hand::HandType rival)
{
	return Result[main][rival];
}

string const Hand::HandName(HandType type)
{
	return m_HandName[type];
}

