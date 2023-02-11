#include "main.h"
#include "Square.h"
#include "Board.h"
#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "Client.h"
#include "Judgement.h"

#include "PieceBehaviour.h"
#include "ClientBehaviour.h"
#include "NetworkManager.h"
#include "SceneManager.h"

#include "UIThinkMark.h"
#include "WorldToScreen.h"

Client::Client()
{
	//reset room data
	JoinedClientNum = 0;
	TargetClientNum = 0;
	memset(&ServerAddr, 0, LEN_ADDRIN);
		
	//create behaviour
	BH_Init = new ClientInit(this);
	BH_Uninit = new ClientUninit(this);
	BH_SelectServer = new ClientSelectServer(this);
	BH_WaitPiecesFinish = new ClientWaitPiecesFinish(this);
	BH_ShowStep = new ClientShowStep(this);
}

Client::~Client()
{
	//delete behaviour by base class destructor
}

void Client::SendChatMsg()
{
	if (!JoinSuccess)
	{
		return;
	}

	//send
	std::string msg =
		"[" + std::to_string(ID) + "]" +
		Name + std::string(": ") +
		Chat;
	NetworkManager::Instance()->Client_SendTestMsgToBroadcast(msg.c_str());
}

void Client::JoinRoomAndSetID(int id)
{
	if (!JoinSuccess)
	{
		DebugInfo::Print("join room and get id" +
			std::string("[" + std::to_string(id) + "]"));

		JoinSuccess = true;
		ID = id;
		State = (int)Client::State_SelectServer::FINISH_JOIN;
	}
}

void Client::UpdateNum(int joinedNum, int targetNum)
{
	DebugInfo::Print("update joined and target player num");

	JoinedClientNum = joinedNum;
	TargetClientNum = targetNum;
}

void Client::RequestDisconnect()
{
	NetworkManager::Instance()->Client_RequestDisconnect();
}

void Client::Disconnect()
{
	DebugInfo::Print("server closed");

	//set default val
	JoinedClientNum = 0;
	ServerPort = 5555;
	State = (int)Client::State_SelectServer::NONE;
	memcpy(ServerIP, std::string("127.0.0.1").c_str(), 16);

	//next bh
	StartBH(BH_Uninit);

	//over join  
	//JoinSuccess = false;
}

void Client::InitGameSceneObjects()
{
}

void Client::CreatePlayer(PlayerDesc desc)
{
	DebugInfo::Print(
		"create main player " +
		std::string("[" + std::to_string(desc.ClientID) + "]") +
		"...");
	MainPlayer = new Player();
	MainPlayer->Camp = desc.PlayerCamp;
	MainPlayer->ID = desc.ClientID;
}

void Client::CreatePiece(PieceDesc desc)
{
	std::string info = 
		"create piece " +
		std::string("[" + std::to_string(desc.PieceID) + "]") +
		"...";
	int printRowID = DebugInfo::Print(info);

	Piece* piece = new Piece();
	{
		piece->ID = desc.PieceID;
		piece->Camp = desc.PieceCamp;
		{
			//classify pieces by camp
			if (piece->Camp == CampType::BAD)
			{
				Bads.emplace_back(piece);
			}
			else if (piece->Camp == CampType::GOOD)
			{
				Goods.emplace_back(piece);
			}
		}
		piece->OwnCharacter = Character::Create(desc.CharaType);

		if (desc.PlayerID == MainPlayer->ID)
			//add self player's piecce
		{
			piece->OwnerPlayer = MainPlayer;
			MainPlayer->AddSelfPiece(piece);
			DebugInfo::PrintRow(printRowID, info+"[self]");
		}
	}
	Pieces.emplace(piece->ID,piece);
}

void Client::ChangeToGameScene()
{
	SceneManager::ChangeScene<GameScene>();
}

void Client::ClearPiecesFinishMark()
{
	DebugInfo::Print("clear pieces finish mark...");
	for (auto idPiece : Pieces)
	{
		Piece* piece = idPiece.second;
		if (piece->FinishMark)
		{
			piece->FinishMark->SetState(GameObject::DEAD);
			piece->FinishMark = nullptr;
		}
	}
}

void Client::ShowPiecesThinkMark()
{
	//show rival piece's think mask directly
	DebugInfo::Print("show pieces think mark...");
	for (auto idPiece : Pieces)
	{
		Piece* piece = idPiece.second;
		if (piece->OwnerPlayer)
			//self piece
		{
			piece->SetUISelect(true);
		}
		else
			//other player's piece
		{
			piece->SetUIThink(true);
		}
	}
}


void Client::StartWaitPiecesFinish()
{
	//tips: check just start wait once 
	if (BH_WaitPiecesFinish->Working)
	{
		return;
	}
	else
	{
		//clear pieces finish mark
		ClearPiecesFinishMark();

		DebugInfo::Print("");
		DebugInfo::Print("- start WaitPiecesFinish");
		BH_WaitPiecesFinish->PrintCount("count");

		BH_WaitPiecesFinish->Reset(
			Pieces.size(),
			[this]() {
				DebugInfo::Print("- finish WaitPiecesFinish");
				NetworkManager::Instance()->Client_NotifyCountPlayerFinished(); });
		StartBH(BH_WaitPiecesFinish);
	}
}

void Client::StartPieceShowEntry(int pieceID, int squareID)
{
	Piece* piece = Pieces[pieceID];
	Square* square = Board::Instance()->Squares[squareID];

	//set piece and square
	square->Piece = piece;
	piece->FootOnSquare = square;

	//create piece obj
	piece->m_PieceObject = new PieceObject();
	//if is self piece, set color blue
	piece->m_PieceObject->SetTexColor(
		piece->OwnerPlayer!=nullptr?
		D3DXVECTOR4(0, 0, 1, 1) :
		D3DXVECTOR4(1, 1, 1, 1)
	);
	piece->m_PieceObject->GetTransform()->SetPosition(0,1000,0);//dont show at first

	//start show entry bh
	PieceShowEntry* showEntry = new PieceShowEntry(piece);
	showEntry->SetEndEvent(
		[this]() 
		{
			BH_WaitPiecesFinish->FinishPieceNum++;
			BH_WaitPiecesFinish->PrintCount("piece show entry");
		});
	piece->StartBH(showEntry);
}

void Client::StartPieceShowHand(int pieceID)
{
	Piece* piece = Pieces[pieceID];

	//start show entry bh
	PieceShowHand* showHand = new PieceShowHand(piece);
	showHand->SetEndEvent(
		[this]() 
		{
			BH_WaitPiecesFinish->FinishPieceNum++;
			BH_WaitPiecesFinish->PrintCount(" piece show hand");
		});
	piece->StartBH(showHand);
}

void Client::StartPieceShowCheckActpoint(int pieceID)
{
	Piece* piece = Pieces[pieceID];
	DebugInfo::Print(
		"piece " +
		std::to_string(piece->ID) +
		" [self] start input act...");

	//create ui show actpoint
	if (!piece->UIShowActpoint)
	{ 
		UIActPoint* uiActpoint = new UIActPoint();
		Transform3D* follow = piece->m_PieceObject->
			GetUITransform(PieceObject::UITID::PAWN_UP_RIGHT)->GetTransform();
		uiActpoint->FollowWorldObject->SetTargetTransform(follow);

		piece->UIShowActpoint = uiActpoint;
	}
	
	//start check actpoint bh
	PieceShowCheckActpoint* showCheckActpoint = new PieceShowCheckActpoint(piece);
	showCheckActpoint->SetEndEvent(
		[this]()
		{
			//wrong
			//NetworkManager::Instance()->Client_NotifyCountPieceFinished();

			NetworkManager::Instance()->Client_NotifyCountPlayerFinished();
		});
	piece->StartBH(showCheckActpoint);
}

void Client::StartPieceInputAct(int pieceID)
{
	Piece* piece = Pieces[pieceID];
	MainPlayer->CheckSelfPieceToInputAct(piece);
}

void Client::StartPiecesClearHandAndActpoint()
{
	for (auto idPiece : Pieces)
	{
		Piece* piece = idPiece.second;

		//clear actpoint
		PieceClearActPoint* clearActpoint = new PieceClearActPoint(piece);
		clearActpoint->SetEndEvent([this,piece]()
			{
				//cleat hand
				PieceClearHand* clearHand = new PieceClearHand(piece);
				clearHand->SetEndEvent([this]()
					{
						BH_WaitPiecesFinish->FinishPieceNum++;
						BH_WaitPiecesFinish->PrintCount(" piece clear hand and actpoint");
					});
				piece->StartBH(clearHand);
			});
		piece->StartBH(clearActpoint);
	}
}



void Client::StartPieceShowFinishAct(int pieceID)
{
	Piece* piece = Pieces[pieceID];

	//update mark UI
	{
		piece->SetUIFinish(true);
		piece->SetUIThink(false);
		piece->SetUISelect(false);
	}

	//notify server judgement show finish act over
	NetworkManager::Instance()->Client_NotifyCountPlayerFinished();
}

void Client::StartPieceShowMove(int pieceID, int squareID)
{
	Piece* piece = Pieces[pieceID];

}


void Client::StartWaitShowStep(int stepType)
{
	BH_ShowStep->Reset(stepType,[]() 
		{
			NetworkManager::Instance()->Client_NotifyCountPlayerFinished();
		});
	StartBH(BH_ShowStep);
}


