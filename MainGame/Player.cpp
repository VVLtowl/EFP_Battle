#include "main.h"
#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "Client.h"

#include "PlayerBehaviour.h"
#include "Judgement.h"
#include "NetworkManager.h"
#include "UIThinkMark.h"
#include "WorldToScreen.h"

Player::Player()
{
	BH_WaitSelfPieces = new PlayerWaitSelfPiecesFinish(this);
	BH_WaitSelfPiecesInputHand = new PlayerWaitSelfPiecesInputHand(this);
}

void Player::ClearDataInGameScene()
{
	m_SelfPieces.Clear();
}

void Player::AddSelfPiece(Piece* piece)
{
	m_SelfPieces.Add(piece);
}

void Player::StartIterateSelfPiecesInputHand()
{
	//set up and start wait bh
	BH_WaitSelfPiecesInputHand->SetEndEvent("NotifyCountPlayerFinished",
		[this]() {
		//next
		NetworkManager::Instance()->Client_NotifyCountPlayerFinished();
		});
	StartBH(BH_WaitSelfPiecesInputHand);
}

void Player::CheckSelfPieceToInputAct(Piece* piece)
{
	if (piece->m_OwnerPlayer)
		//is self piece
	{
		DebugInfo::Print(
			"piece " +
			std::to_string(piece->m_ID) +
			" [self] start input act...");

		//start piece input act
		piece->StartInputSelectAct();
	}
	else
		//other player's piece
	{
		DebugInfo::Print(
			"piece " +
			std::to_string(piece->m_ID) +
			" start input act...");

		//show think mark
		piece->SetUIThink(true);

		//reset camera pos
		Client::Instance()->ResetCameraLookAt();
	}
}

