#include "main.h"
#include "BehaviourFactory.h"
#include "WorldToScreen.h"
#include "GameManager.h"
#include "JBH_WaitPlayersRPS.h"

#include "PBH_ShowFinish.h"
#include "PBH_RPS.h"

void JBH_WaitPlayersRPS::Start()
{
	m_PlayerEnd = GameManager::Instance()->Players.end();
	m_PlayerIterator = GameManager::Instance()->Players.begin();
	WaitState = START_PLAYER_RPS;
}

void JBH_WaitPlayersRPS::MainUpdate()
{
	if (m_PlayerIterator == m_PlayerEnd)
	{
		for (auto player : GameManager::Instance()->Players)
		{
			player->PieceData.FinishMark->SetState(GameObject::DEAD);
		}
		NextState();
	}
	else
	{
		Player* player = (*m_PlayerIterator);

		if (WaitState == START_PLAYER_RPS)
		{
			//add act mark to current rps player
			UIActMark* actMark = GameObjectManager::Create<UIActMark>();
			Transform3D* parent =
				player->PieceData.OwnPieceObject->
				GetUITransform(PieceObject::UITID_PAWN_CENTER);
			actMark->GetTransform()->SetParent(parent);
			player->PieceData.ActMark = actMark;

			//jump to playerBH
			PBH_RPS* rps = BehaviourFactory::CreatePBH<PBH_RPS>(player, true);
			m_Excuter->JumpTo(rps);

			//set state
			WaitState = PLAYER_RPS_FINISH;
		}
		else if (WaitState == PLAYER_RPS_FINISH)
		{
			//remove actmark from finish rps player
			player->PieceData.ActMark->SetState(GameObject::DEAD);
			player->PieceData.ActMark = nullptr;

			//jump to player show finish 
			Behaviour* showPlayerFinish =
				BehaviourFactory::CreatePBH<PBH_ShowFinish>(player, true);
			m_Excuter->JumpTo(showPlayerFinish);

			//increment iterator and set state
			m_PlayerIterator++;
			WaitState = START_PLAYER_RPS;
		}
	}
}
