#include "main.h"

#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "PlayerBehaviour.h"

#include "LoopList.h"

#pragma region ========== wait self pieces finish ==========

void PlayerWaitSelfPiecesFinish::Update()
{
	if (FinishPieceCount == TargetPieceCount)
	{
		NextState();
	}
}

void PlayerWaitSelfPiecesFinish::Reset(int targetNum)
{
	FinishPieceCount = 0;
	TargetPieceCount = targetNum;
}

#pragma endregion

#pragma region ========== wait self piece input hand ==========

void PlayerWaitSelfPiecesInputHand::Start()
{
	//reset state
	{
		WaitState = State::CHECK_START_PIECE;
	}
}

void PlayerWaitSelfPiecesInputHand::Update()
{
	switch (WaitState)
	{
	case State::CHECK_START_PIECE://execute one frame
	{
		//check first one
		Piece* piece = CheckEnablePiece(CheckType::CHECK_FIRST);
		if (!piece)
		{
			WaitState = State::FINISH;
			break;
		}

		//set up piece input hand BH
		piece->StartInputHand([this]()
			{
				InputFinish = true;
			});

		//next state
		WaitState = State::WAIT_INPUT;
		break;
	}
	case State::CHECK_NEXT_PIECE://execute one frame
	{
		//check next one
		Piece* piece = CheckEnablePiece(CheckType::CHECK_NEXT);
		if (!piece)
		{
			WaitState = State::FINISH;
			break;
		}

		//set up piece input hand BH
		piece->StartInputHand([this]()
			{
				InputFinish = true;
			});

		//next state
		WaitState = State::WAIT_INPUT;
		break;
	}
	case State::WAIT_INPUT://wait update frame
	{
		//wait now piece finish input hand
		if (InputFinish)
		{
			InputFinish = false;
			WaitState = State::CHECK_NEXT_PIECE;
		}
		break;
	}
	case State::FINISH://execute one frame
	{
		NextState();
		break;
	}
	}
}

Piece* PlayerWaitSelfPiecesInputHand::CheckEnablePiece(CheckType checkType)
{
	//try check piece enable
	Piece* piece = nullptr;
	if (checkType == CheckType::CHECK_FIRST)
	{
		piece = m_Player->SelfPieces.First();
	}
	else if (checkType == CheckType::CHECK_NEXT)
	{
		piece = m_Player->SelfPieces.Next();
	}
	else if (checkType == CheckType::CHECK_PREV)
	{
		piece = m_Player->SelfPieces.Prev();
	}

	//if piece finished, try get other piece
	int count = 0;
	int countMax = m_Player->SelfPieces.GetList()->size();
	if (piece)
	{
		//test
		while (piece->FinishMark && count < countMax)
		{
			if (checkType == CheckType::CHECK_PREV)
			{
				piece = m_Player->SelfPieces.Prev();
			}
			else
			{
				piece = m_Player->SelfPieces.Next();
			}
			count++;
		}
	}
	
	//if all pieces finished
	if(count>=countMax)
	{
		piece = nullptr;
	}

	//set now operate piece
	m_Player->NowOperatePiece = piece;
	return piece;
}

#pragma endregion

