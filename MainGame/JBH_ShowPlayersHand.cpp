#include "main.h"
#include "JBH_ShowPlayersHand.h"


#include "Judgement.h"
#include "TransformAnime.h"

const int SHOW_PLAYERS_HAND_DURATION = 10;//25

void JBH_ShowPlayersHand::Start()
{
	m_PlayerEnd = GameManager::Instance()->Players.end();
	m_PlayerIterator = GameManager::Instance()->Players.begin();

	m_AniOverCount = 0;
	m_PlayerAmount = GameManager::Instance()->Players.size();

	m_ShowUIFrameCount = 0;
	m_ShowUIDuration = 1;
}

void JBH_ShowPlayersHand::MainUpdate()
{
	if (m_PlayerIterator == m_PlayerEnd)
	{
		if (m_AniOverCount >= m_PlayerAmount)
		{
			NextState();
		}
	}
	else
	{
		if (m_ShowUIFrameCount < m_ShowUIDuration)
		{
			m_ShowUIFrameCount++;
		}
		else
		{
			m_ShowUIFrameCount = 0;

			Player* player = (*m_PlayerIterator);

			//set player's hand ui
			UIRPS* uiObject = GameObjectManager::Create<UIRPS>();
			player->PieceData.UIHand = uiObject;//for delete
			uiObject->SetRPS(player->PieceData.Hands.back());
			Transform3D* parent =
				player->PieceData.OwnPieceObject->
				GetUITransform(PieceObject::UITID_PAWN_UP_LEFT);
			uiObject->GetTransform()->SetParent(parent);

			//add ui anime
			{
				Animator* animator = new Animator(uiObject);
				animator->AddEndEvent([this]() 
					{
						m_AniOverCount++;
					});

				PositionHermiteDesc posDesc;
				ComputeHermiteVec3 posComputeFunc;
				posDesc.Duration = SHOW_PLAYERS_HAND_DURATION;
				posDesc.StartPosition = { 0,-0.3f,0 };
				posDesc.EndPosition = {0,0,0};
				posDesc.StartTangent = {0,1,0};
				posDesc.EndTangent = { 0,-1,0 };
				new PositionAnime(animator, posDesc, posComputeFunc);

				Vec3StartEndDesc sclDesc;
				ComputeUniformVec3 sclComputeFunc;
				sclDesc.Duration = SHOW_PLAYERS_HAND_DURATION;
				sclDesc.Start = V3_ZERO;
				sclDesc.End = V3_ONE * 0.8f;
				new ScaleAnime(animator, sclDesc, sclComputeFunc);
			}

			m_PlayerIterator++;
		}
	}
}
