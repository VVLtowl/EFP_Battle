#include "main.h"
#include "GameManager.h"
#include "TransformAnime.h"
#include "JBH_EntryPieces.h"

void JBH_EntryPieces::Start()
{
	m_PlayerEnd = GameManager::Instance()->Players.end();
	m_PlayerIterator = GameManager::Instance()->Players.begin();
	m_AniOverCount = 0;
	m_PlayerAmount = GameManager::Instance()->Players.size();

	m_EntryFrameCount = 0;
	m_EntryDuration = 1;
}

void JBH_EntryPieces::MainUpdate()
{
	//全員登場までは、次のステートへ行かない
	if (m_PlayerIterator == m_PlayerEnd)
	{
		if (m_AniOverCount >= m_PlayerAmount)
		{
			NextState();
		}
	}
	else
	{
		if (m_EntryFrameCount < m_EntryDuration)
		{
			m_EntryFrameCount++;
		}
		else
		{
			m_EntryFrameCount = 0;

			Player* player = (*m_PlayerIterator);
			Piece* piece = &player->PieceData;
			Square* startSquare = piece->Camp == CAMP_BAD ?
				GameManager::Instance()->MainBoard.GetBadStartSquare() :
				GameManager::Instance()->MainBoard.GetGoodStartSquare();

			piece->FootOnSquare = startSquare;
			startSquare->Piece = piece;
			D3DXVECTOR3 startSquarePos =
				startSquare->TargetSquareObject->GetTransform()->GetPosition();

			//コマ登場の演出を始める
			{
				Animator* animator = new Animator(piece->OwnPieceObject);
				animator->AddEndEvent([this]() {	m_AniOverCount++; });
				Vec3StartEndDesc desc;
				desc.Duration = 20;
				desc.Start = startSquarePos + D3DXVECTOR3(0, 10, 0); 
				desc.End = startSquarePos + D3DXVECTOR3(0, 0.25f, 0);
				ComputeSlowStartSlowEndVec3 computeFunc;
				PositionAnime* anime = new PositionAnime(animator, desc, computeFunc);
			}

			m_PlayerIterator++;
		}
	}
}

void JBH_EntryPieces::End()
{
	delete this;
}
