#include "main.h"
#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "Judgement.h"
#include "JudgementBehaviour.h"

#include "NetworkManager.h"

#include "imgui/imgui.h"



JudgementBehaviour::JudgementBehaviour(Judgement* j, bool isOnce):
	m_OwnerJudgement(j),
	Behaviour(j,isOnce)
{
}


#pragma region ========== input game room ==========

void JudgementInputPlayMode::Start()
{
	//test
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_JUDGEMENT_INPUTPLAYMODE,
		[this]() {

		}
	);
}

void JudgementInputPlayMode::Update()
{
	if (SetUpMode)
	{
		//next
	}
}

#pragma endregion

#pragma region ========== wait players finish ==========

void JudgementWaitPlayersFinish::Reset()
{
	FinishPlayerNum = 0;
	TargetPlayerNum = GameManager::m_Players.size();
	PrintRowID = -1;
}

void JudgementWaitPlayersFinish::Update()
{
	if (FinishPlayerNum == TargetPlayerNum)
	{
		//go to next bh
		NextState();
	}
}

void JudgementWaitPlayersFinish::PrintCount(std::string info)
{
	if (PrintRowID == -1)
	{
		PrintRowID = DebugInfo::Print(
			"[" +
			std::to_string(FinishPlayerNum) +
			"/" +
			std::to_string(TargetPlayerNum) +
			"]" +
			info);
	}
	else
	{
		DebugInfo::PrintRow(PrintRowID,
			"[" +
			std::to_string(FinishPlayerNum) +
			"/" +
			std::to_string(TargetPlayerNum) +
			"]" +
			info);
	}
}

#pragma endregion

#pragma region ========== wait pieces finish ==========

void JudgementWaitPiecesFinishCommand::CheckPiecesOrder()
{
	//test
	//good pieces act after bad pieces 
	//add bad pieces to orderlist at first
	PieceActOrder.clear();
	for (auto piece : GameManager::m_Bads)
	{
		PieceActOrder.emplace_back(piece);
	}
	for (auto piece : GameManager::m_Goods)
	{
		PieceActOrder.emplace_back(piece);
	}
}

void JudgementWaitPiecesFinishCommand::Reset(std::function<void()> cmdEvent)
{
	//reset val
	{
		PrintRowID = -1;
		FinishPieceNum = 0;

		TargetPieceNum = 0;
		for (auto idPiece : GameManager::m_Pieces)
		{
			Piece* piece = idPiece.second;
			if (piece->m_State == Piece::State::NORMAL)
			{
				TargetPieceNum++;
			}
		}
	}

	//set command event
	OnCommand = cmdEvent;

	//init piece iterator
	{
		CheckPiecesOrder();
		PieceIterator = PieceActOrder.begin();
		PieceEnd = PieceActOrder.end();
	}

	//init state
	WaitState = State::CHECK_COMMAND_PIECE;
}


void JudgementWaitPiecesFinishCommand::Update()
{
	switch (WaitState)
	{
	case State::CHECK_COMMAND_PIECE:
	{
		//check all pieces finish
		if (PieceIterator == PieceEnd)
		{
			//next state
			WaitState = State::ALL_PIECES_FINISH;
		}
		else
		{
			//command event
			OnCommand();

			//tips: set next state state in OnCommand
			//next state
			//WaitState = State::WAIT_PIECE_FINISH_COMMAND;
		}

		break;
	}
	case State::WAIT_PIECE_FINISH_COMMAND:
	{
		//wait clients piece check actpoint finish
		break;
	}
	case State::PIECE_FINISH_COMMAND:
	{
		//update show num
		PrintCount("piece finish command");
		FinishPieceNum++;

		//iterate
		PieceIterator++;

		//tips: check piece state in OnCommand
		/*while (PieceIterator != PieceEnd)
		{
			if((*PieceIterator)->m_State == Piece::State::CAUGHT)
			{
				PieceIterator++;
			}
			else if((*PieceIterator)->m_State == Piece::State::NORMAL)
			{
				break;
			}
		}*/

		//next state
		WaitState = State::CHECK_COMMAND_PIECE;
		break;
	}
	case State::ALL_PIECES_FINISH:
	{
		//finish
		NextState();
		break;
	}
	}
}

void JudgementWaitPiecesFinishCommand::PrintCount(std::string info)
{
	if (PrintRowID == -1)
	{
		PrintRowID = DebugInfo::Print(
			"[" +
			std::to_string(FinishPieceNum) +
			"/" +
			std::to_string(TargetPieceNum) +
			"]" +
			info);
	}
	else
	{
		DebugInfo::PrintRow(PrintRowID,
			"[" +
			std::to_string(FinishPieceNum) +
			"/" +
			std::to_string(TargetPieceNum) +
			"]" +
			info);
	}
}

#pragma endregion

#pragma region ========== loop command clients show pieces ==========

void JudgementCommandShowPieces::Update()
{
	if (FinishCount >= TargetCount)
	{
		//finish
		NextState();
	}
	else
	{
		if (FrameCount >= Duration)
		{
			//send command
			OnCommand();

			//count
			FrameCount = 0;
			FinishCount++;
			PieceIterator++;
		}
		else
		{
			FrameCount += FRAME_STRIDE;
		}
	}
}

void JudgementCommandShowPieces::Reset(int duration, int targetCount, std::function<void()> cmdEvent)
{
	//init val
	{
		FrameCount = 0;
		Duration = duration;

		FinishCount = 0;
		TargetCount = targetCount;//m_OwnerJudgement->Pieces.size();
	}

	//init piece iterator
	{
		PieceIterator = GameManager::m_Pieces.begin();
		PieceEnd = GameManager::m_Pieces.end();
	}

	//set command event
	{
		OnCommand = cmdEvent;
	}

	//reset wait
	{
		m_OwnerJudgement->BH_WaitPlayersFinish->Reset();
	}
}

#pragma endregion
