#include "main.h"
#include "Square.h"
#include "Board.h"
#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "Client.h"
#include "Server.h"
#include "Judgement.h"

#include "PieceBehaviour.h"
#include "ClientBehaviour.h"
#include "NetworkManager.h"
#include "SceneManager.h"

#include "UIThinkMark.h"

#include"TransformAnime.h"
#include "WorldToScreen.h"
#include "LookAt.h"

Client::Client()
{
	//reset room data
	memset(&m_ServerAddr, 0, LEN_ADDRIN);
	m_TargetClientNum = 0;
		
	//create behaviour
	BH_Init = new ClientInit(this);
	BH_Uninit = new ClientUninit(this);
	BH_SelectServer = new ClientSelectServer(this);
	BH_TryConnectServer = new ClientTryConnectServer(this);
	BH_WaitStartRoom = new ClientWaitRoom(this);

	BH_WaitPiecesFinish = new ClientWaitPiecesFinish(this);
	BH_ShowStep = new ClientShowStep(this);
	BH_ShowGameOver = new ClientShowGameOver(this);
}

Client::~Client()
{
	//delete behaviour by base class destructor
}

void Client::SendChatMsg()
{
	if (!m_JoinSuccess)
	{
		return;
	}

	//send
	std::string msg =
		"[" + std::to_string(m_ID) + "]" +
		m_Name + std::string(": ") +
		m_Chat;
	NetworkManager::Instance()->Client_SendTestMsgToBroadcast(msg.c_str());
}

void Client::StartInWaitRoomScene()
{
	if (m_JoinSuccess)
	{
		//notify server judgement load wait room scene finish
		NetworkManager::Instance()->Client_NotifyCountPlayerFinished();

		//start wait room
		StartWaitRoom();
	}
	else
	{
		StartSelectServer();
	}
}

void Client::StartInGameScene()
{
	//init board
	Board::Instance()->Init();

	//reset camera and light's look at
	LightManager::GetMainLight()->LightLookAt->m_TargetTransform =
		CameraManager::GetMainCamera()->m_CmrLookAt->m_TargetTransform =
		Board::Instance()->m_BoardCenter->GetTransform();

	//notify server judgement load gamescene finish
	NetworkManager::Instance()->Client_NotifyCountPlayerFinished();

	//wait server judgement command
}

void Client::StartSelectServer()
{
	//set up select server
	BH_SelectServer->Reset();
	BH_SelectServer->SetEndEvent("StartClientWaitRoom", 
		[this]()
		{
			if (BH_SelectServer->m_SelectState == 
				ClientSelectServer::State::FINISH_JOIN)
			{
				StartWaitRoom();
			}
			else if (BH_SelectServer->m_SelectState ==
				ClientSelectServer::State::FINISH_EXIT)
			{

			}
		});
	StartBH(BH_SelectServer);
}

void Client::TryJoinServer()
{
	if (NetworkManager::Instance()->IsTCP)
		//tcp need connect before send join request
	{
		BH_TryConnectServer->Reset();
		BH_TryConnectServer->SetEndEvent("CheckConnect ToRequestOrBack",
			[this]()
			{
				if (BH_TryConnectServer->m_ConnectState ==
					ClientTryConnectServer::State::CONNECT_SUCCEED)
				{
					//BH_TryConnectServer back to BH_SelectServer
					

					//BH_SelectServer jump to ClientRequestJoinServer
					JumpToBH(new ClientRequestJoinServer(this));
				}
				else if (BH_TryConnectServer->m_ConnectState ==
					ClientTryConnectServer::State::CONNECT_TIME_OUT)
				{
					StartSelectServer();
				}
			});

		//BH_SelectServer jump to BH_TryConnectServer
		JumpToBH(BH_TryConnectServer);
	}
	else
		//udp can send join request directly
	{
		//BH_SelectServer jump to ClientRequestJoinServer
		JumpToBH(new ClientRequestJoinServer(this));
	}
}

void Client::CheckConnect()
{
	if (m_ExecuteBehaviour == BH_TryConnectServer)
	{
		BH_TryConnectServer->m_ConnectState =
			ClientTryConnectServer::State::CHECK_CONNECT;
	}
}

void Client::JoinRoom(int id, int targetNum)
{
	if (!m_JoinSuccess)
	{
		DebugInfo::Print(
			"join room set ID[" + std::to_string(id) + "]" +
			"set target client num[" + std::to_string(targetNum) + "]");

		//set value
		m_JoinSuccess = true;
		m_ID = id;
		m_TargetClientNum = targetNum;

		//set SelectServerBH state
		BH_SelectServer->m_SelectState = ClientSelectServer::State::FINISH_JOIN;
	}
}

void Client::StartWaitRoom()
{
	BH_WaitStartRoom->Reset();
	BH_WaitStartRoom->SetEndEvent("ChangeScene ToGameScene",
		[this]()
		{
			SceneManager::ChangeScene<GameScene>();
		});
	StartBH(BH_WaitStartRoom);
}

void Client::UpdateClientInfo(int checkMemID,std::string name,bool ready)
{
	DebugInfo::Print("update joined and target client info");

	//check if contain member
	if (m_ClientInfos.find(checkMemID) !=
		m_ClientInfos.end())
		//contain member
	{
		ClientMember* mem = m_ClientInfos[checkMemID];
		mem->Ready = ready;
	}
	else
		//new member
	{
		ClientMember* mem = new ClientMember(checkMemID, name);
		mem->Ready = ready;

		//add to list
		m_ClientInfos.emplace(checkMemID, mem);
	}
}

void Client::RequestDisconnect()
{
	NetworkManager::Instance()->Client_RequestDisconnect();
}

void Client::Disconnect()
{
	DebugInfo::Print("server closed");

	//set default val
	m_ServerPort = 5555;
	BH_SelectServer->m_SelectState = ClientSelectServer::State::NONE;
	memcpy(m_ServerIP, std::string("127.0.0.1").c_str(), 16);

	//next bh
	StartBH(BH_Uninit);

	//over join  
	//JoinSuccess = false;
}


void Client::ResetCameraLookAt(float duration)
{
	//reset camera pos
	{
		Camera* cmr = CameraManager::GetMainCamera();
		cmr->m_CmrLookAt->m_TargetTransform = Board::Instance()->m_BoardCenter->GetTransform();
		cmr->m_IsOrtho = true;

		//calculate end pos and look at
		D3DXVECTOR3 endPos = { 10, 10, -10 };

		Animator* animator = new Animator(cmr->GetGameObject());
		AniDesc_Vec3StartEnd desc;
		{
			desc.Duration = duration;
			desc.Start = cmr->GetGameObject()->GetTransform()->GetWorldPosition();
			desc.End = endPos;
		}
		ComputeSlowLerpVec3 computeFunc;
		new Anime_Position(animator, desc, computeFunc);
	}
}

void Client::CreatePlayer(PlayerDesc desc)
{
	DebugInfo::Print(
		"create main player " +
		std::string("[" + std::to_string(desc.ClientID) + "]") +
		"...");
	m_MainPlayer = new Player();
	m_MainPlayer->m_Camp = desc.PlayerCamp;
	m_MainPlayer->m_ID = desc.ClientID;
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
		piece->m_ID = desc.PieceID;
		piece->m_Camp = desc.PieceCamp;
		{
			//classify pieces by camp
			if (piece->m_Camp == CampType::BAD)
			{
				GameManager::m_Bads.emplace_back(piece);
			}
			else if (piece->m_Camp == CampType::GOOD)
			{
				GameManager::m_Goods.emplace_back(piece);
			}
		}
		piece->m_OwnCharacter = Character::Create(desc.CharaType);

		if (desc.PlayerID == m_MainPlayer->m_ID)
			//add self player's piecce
		{
			piece->m_OwnerPlayer = m_MainPlayer;
			m_MainPlayer->AddSelfPiece(piece);
			DebugInfo::PrintRow(printRowID, info+"[self]");
		}
	}
	GameManager::m_Pieces.emplace(piece->m_ID,piece);
}

void Client::ChangeToGameScene()
{
	if (m_ExecuteBehaviour == BH_WaitStartRoom)
	{
		BH_WaitStartRoom->m_WaitState =
			ClientWaitRoom::State::FINISH;
	}
}

void Client::ClearPiecesFinishMark()
{
	DebugInfo::Print("clear pieces finish mark...");
	for (auto idPiece : GameManager::m_Pieces)
	{
		Piece* piece = idPiece.second;
		if (piece->m_FinishMark)
		{
			piece->m_FinishMark->SetState(GameObject::DEAD);
			piece->m_FinishMark = nullptr;
		}
	}
}

void Client::ClearDataInGameScene()
{
	//uninit board
	Board::Instance()->Uninit();

	//clear players
	if (m_MainPlayer)
	{
		m_MainPlayer->ClearDataInGameScene();
		delete m_MainPlayer;
	}

	//clear pieces list
	{
		GameManager::m_Goods.clear();
		GameManager::m_Bads.clear();

		for(auto idPiece: GameManager::m_Pieces)
		{
			delete idPiece.second;
		}
		GameManager::m_Pieces.clear();
	}
}

void Client::ShowPiecesThinkMark()
{
	//show rival piece's think mask directly
	DebugInfo::Print("show pieces think mark...");
	for (auto idPiece : GameManager::m_Pieces)
	{
		Piece* piece = idPiece.second;
		if (piece->m_OwnerPlayer)
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
			GameManager::m_Pieces.size(),
			"NotifyCountPlayerFinished",
			[this]() 
			{
				DebugInfo::Print("- finish WaitPiecesFinish");
				NetworkManager::Instance()->Client_NotifyCountPlayerFinished(); 
			});
		StartBH(BH_WaitPiecesFinish);
	}
}

void Client::StartPieceShowEntry(int pieceID, int squareID)
{
	Piece* piece = GameManager::m_Pieces[pieceID];
	Square* square = Board::Instance()->Squares[squareID];

	//set piece and square
	square->m_Piece = piece;
	piece->m_FootOnSquare = square;

	//create piece obj
	piece->m_PieceObject = new PieceObject();
	if (piece->m_Camp == CampType::BAD)
	{
		piece->m_PieceObject->m_PawnImage->m_Polygon2D->Texture =
			DrawManager::Textures[TEXID_TEST_PIECE_NORMALBAD];
	}
	else if (piece->m_Camp == CampType::GOOD)
	{
		piece->m_PieceObject->m_PawnImage->m_Polygon2D->Texture =
			DrawManager::Textures[TEXID_TEST_PIECE_NORMALGOOD];
	}
	//if is self piece, set color blue
	piece->m_PieceObject->SetTexColor(
		piece->m_OwnerPlayer!=nullptr?
		D3DXVECTOR4(1, 2, 1, 1) :
		D3DXVECTOR4(1, 1, 1, 1)
	);
	piece->m_PieceObject->GetTransform()->SetPosition(0,1000,0);//dont show at first

	//start show entry bh
	PieceShowEntry* showEntry = new PieceShowEntry(piece);
	showEntry->SetEndEvent("CountPieceFinish ShowEntry", [this]()
		{
			BH_WaitPiecesFinish->FinishPieceNum++;
			BH_WaitPiecesFinish->PrintCount("piece show entry");
		});
	piece->StartBH(showEntry);
}

void Client::StartPieceShowHand(int pieceID)
{
	Piece* piece = GameManager::m_Pieces[pieceID];

	//start show entry bh
	PieceShowHand* showHand = new PieceShowHand(piece);
	showHand->SetEndEvent("CountPieceFinish ShowHand",[this]() 
		{
			BH_WaitPiecesFinish->FinishPieceNum++;
			BH_WaitPiecesFinish->PrintCount(" piece show hand");
		});
	piece->StartBH(showHand);
}

void Client::StartPieceShowCheckActpoint(int pieceID,int actPoint)
{
	Piece* piece = GameManager::m_Pieces[pieceID];
	DebugInfo::Print(
		"piece " +
		std::to_string(piece->m_ID) +
		" [self] start input act...");

	//create ui show actpoint
	piece->SetUIActpoint(true);
	
	//start check actpoint bh
	PieceShowCheckActpoint* showCheckActpoint = new PieceShowCheckActpoint(piece,actPoint);
	showCheckActpoint->SetEndEvent("NotifyCountPlayerFinish CheckActpt",
		[this]()
		{
			NetworkManager::Instance()->Client_NotifyCountPlayerFinished();
		});
	piece->StartBH(showCheckActpoint);
}

void Client::StartPieceInputAct(int pieceID)
{
	Piece* piece = GameManager::m_Pieces[pieceID];
	m_MainPlayer->CheckSelfPieceToInputAct(piece);
}

void Client::StartPiecesClearHandAndActpoint()
{
	//test
	ResetCameraLookAt();

	for (auto idPiece : GameManager::m_Pieces)
	{
		Piece* piece = idPiece.second;

		//clear actpoint
		PieceClearActPoint* clearActpoint = new PieceClearActPoint(piece);
		clearActpoint->SetEndEvent("PieceClearHand", [this, piece]()
			{
				//cleat hand
				PieceClearHand* clearHand = new PieceClearHand(piece);
				clearHand->SetEndEvent("CountPieceFinish ClearHandAndActpt",[this]()
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
	DebugInfo::Print("show piece[" + std::to_string(pieceID) + "] finish...");

	Piece* piece = GameManager::m_Pieces[pieceID];

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
	//start piece move
	Piece* piece = GameManager::m_Pieces[pieceID];
	piece->StartMove(squareID);
}

void Client::StartPieceShowCaught(int movePieceID, int caughtPieceID, int prisonRoomSquareID)
{
	//start piece caught
	Piece* movePiece = GameManager::m_Pieces[movePieceID];
	Piece* caughtPiece = GameManager::m_Pieces[caughtPieceID];
	caughtPiece->StartCaught(prisonRoomSquareID);
}

void Client::StartPieceShowEscape(int escapePieceID, int escapeSquareID)
{
	//start piece caught
	Piece* escapePiece = GameManager::m_Pieces[escapePieceID];
	escapePiece->StartEscape(escapeSquareID);
}


void Client::StartWaitShowStep(int stepType)
{
	BH_ShowStep->Reset(stepType,
		"NotifyCountPlayerFinished",
		[]() 
		{
			NetworkManager::Instance()->Client_NotifyCountPlayerFinished();
		});
	StartBH(BH_ShowStep);
}

void Client::StartShowGameOver(int result)
{
	BH_ShowGameOver->Reset((ClientShowGameOver::ResultType)result,
		"ChangeScene ToClientWaitScene",
		[]()
		{
			SceneManager::ChangeScene<ClientWaitScene>();
		});
	StartBH(BH_ShowGameOver);
}


