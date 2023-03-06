#include "main.h"
#include "MyNetManager.h"

#include "Square.h"
#include "Board.h"
#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "Judgement.h"

#include "Server.h"
#include "Client.h"
#include "ServerBehaviour.h"
#include "ClientBehaviour.h"
#include "PieceBehaviour.h"
#include "JudgementBehaviour.h"

#include "NetworkSendFunc.h"



#pragma region ========== data for network ==========
/*********************************************************
* @brief    network data structure and data size
********************************************************/
//len+=0
const size_t LEN_ZERO = 0;

//len+=128
const size_t LEN_CLIENT_NAME = 64;
const size_t LEN_CLIENT_IP = 64;
struct Data_Client
{
	int ID;
	bool Ready;
	bool Disconnect;
	char Name[LEN_CLIENT_NAME];
	char IP[LEN_CLIENT_IP];//for private chat
};
const size_t LEN_DATA_CLIENT = sizeof(Data_Client);

//len+=256
struct Data_Char256
{
	char Sentence[256] = "test msg";
};
const size_t LEN_DATA_CHAR256 = sizeof(Data_Char256);

//lent+=4
struct Data_Int
{
	int Data;
};
const size_t LEN_DATA_INT = sizeof(Data_Int);

//len+=16
struct Data_Int4
{
	int Data[4];
};
const size_t LEN_DATA_INT4 = sizeof(Data_Int4);

//len+=16
typedef PlayerDesc Data_PlayerDesc;
const size_t LEN_DATA_PLAYERDESC = sizeof(Data_PlayerDesc);

//len+=16
typedef PieceDesc Data_PieceDesc;
const size_t LEN_DATA_PIECEDESC = sizeof(Data_PieceDesc);
#pragma endregion


MyClient * NetworkSendFunc::GetMyClient()
{
	return MyNetManager::Instance()->m_TargetAppClient->m_MyClient;
}

MyServer* NetworkSendFunc::GetMyServer()
{
	return MyNetManager::Instance()->m_TargetAppServer->m_MyServer;
}

void NetworkSendFunc::Server_CheckRecv(const MsgContent& msgContent)
{
	switch (msgContent.BHID)
	{
	case MsgContent::TEST_SERVER_BROADCAST:
	{
		Data_Char256* data = (Data_Char256*)msgContent.Data;
		std::string info = "";
		info.append(data->Sentence);
		DebugInfo::PrintNetMsg(info);

		Server_BroadcastTestMsgToShow(data->Sentence);

		break;
	}

	case MsgContent::SERVER_CHECK_CLIENT_JOIN:
	{
		Data_Client* clientData = (Data_Client*)msgContent.Data;
		std::string name = clientData->Name;
		int id = clientData->ID;//tips: tcpClientID=tcpSockID
		AppServer::Instance()->StartCheckClientJoin(id, name);

		break;
	}

	case MsgContent::SERVER_SET_CLIENT_DISCONNECT:
	{
		Data_Int* clientIDData = (Data_Int*)msgContent.Data;
		int id = clientIDData->Data;

		AppServer::Instance()->SetClientDisconnect(id);

		break;
	}

	case MsgContent::SERVER_SET_CLIENT_READY:
	{
		Data_Int4* clientData = (Data_Int4*)msgContent.Data;
		int id = clientData->Data[0];
		bool ready = clientData->Data[1] == 1 ? true : false;//0:not ready, 1:ready
		AppServer::Instance()->SetClientReady(id, ready);

		break;
	}

	case MsgContent::JUDGEMENT_COUNT_PLAYER_FINISHED:
	{
		Judgement::Instance()->BH_WaitPlayersFinish->FinishPlayerNum++;
		Judgement::Instance()->BH_WaitPlayersFinish->PrintCount("count player finish");

		break;
	}

	//wrong logic error!!!!
	case MsgContent::JUDGEMENT_COUNT_PIECE_FINISHED:
	{
		//Judgement::Instance()->BH_WaitPiecesFinishCmd->WaitState =
			//JudgementWaitPiecesFinishCommand::State::PIECE_FINISH_COMMAND;

		break;
	}

	case MsgContent::JUDGEMENT_COMMAND_SET_HAND:
	{
		Data_Int4* data = (Data_Int4*)msgContent.Data;
		int pieceID = data->Data[0];
		HandType handType = (HandType)data->Data[1];

		//save handtype data in judgement
		GameManager::m_Pieces[pieceID]->AddHand(handType);

		//broadcast to client
		Judgement_CommandSetHand(pieceID, handType);

		break;
	}

	case MsgContent::JUDGEMENT_CHECK_PIECE_MOVE:
	{
		Data_Int4* data = (Data_Int4*)msgContent.Data;
		int pieceID = data->Data[0];
		int squareID = data->Data[1];

		Judgement::Instance()->CheckPieceMove(pieceID, squareID);

		break;
	}

	case MsgContent::JUDGEMENT_COMMAND_PIECE_FINISH:
	{
		Data_Int* data = (Data_Int*)msgContent.Data;
		int pieceID = data->Data;

		Judgement::Instance()->StartWaitClientsPieceFinishAct(pieceID);

		break;
	}
	}
	
}

void NetworkSendFunc::Client_CheckRecv(const MsgContent& msgContent)
{
	switch (msgContent.BHID)
	{
	case MsgContent::TEST_SHOW_MSG:
	{
		Data_Char256* data = (Data_Char256*)msgContent.Data;
		std::string info = "";
		info.append(data->Sentence);
		DebugInfo::PrintNetMsg(info);

		break;
	}

	case MsgContent::CLIENT_SET_ID:
	{
		Data_Int* clientIDData = (Data_Int*)msgContent.Data;
		int id = clientIDData->Data;
		AppClient::Instance()->SetClientID(id);

		break;
	}

	case MsgContent::CLIENT_JOIN_ROOM:
	{
		Data_Int4* clientIDData = (Data_Int4*)msgContent.Data;
		int id = clientIDData->Data[0];
		int targetClientNum = clientIDData->Data[1];
		AppClient::Instance()->JoinRoom(id, targetClientNum);

		break;
	}

	case MsgContent::CLIENT_JOIN_ROOM_FAIL:
	{
		DebugInfo::Print("disconnect because room is full");
		AppClient::Instance()->Disconnect();

		break;
	}

	case MsgContent::CLIENT_UPDATE_JOINEDNUM:
	{
		Data_Client* clientData = (Data_Client*)msgContent.Data;
		AppClient::Instance()->UpdateClientInfo(
			clientData->ID,
			clientData->Name,
			clientData->Ready,
			clientData->Disconnect);

		break;
	}

	case MsgContent::CLIENT_DISCONNECT:
	{
		DebugInfo::Print("server closed");
		DebugInfo::Print("disconnect because server closed");
		AppClient::Instance()->Disconnect();

		break;
	}

	case MsgContent::CLIENT_CREATE_PLAYER:
	{
		Data_PlayerDesc* playerDescData = (Data_PlayerDesc*)msgContent.Data;
		PlayerDesc desc = *playerDescData;
		AppClient::Instance()->CreatePlayer(desc);
		break;
	}

	case MsgContent::CLIENT_SET_PIECE_NUM:
	{
		Data_Int* pieceNumData = (Data_Int*)msgContent.Data;
		AppClient::Instance()->BH_WaitPiecesFinish->TargetPieceNum =
			pieceNumData->Data;
		DebugInfo::Print(
			"set target piece num: " +
			std::to_string(pieceNumData->Data));
		Client_NotifyCountPlayerFinished();
		break;
	}

	case MsgContent::CLIENT_CREATE_PIECE:
	{
		Data_PieceDesc* pieceDescData = (Data_PieceDesc*)msgContent.Data;
		PieceDesc desc = *pieceDescData;
		AppClient::Instance()->CreatePiece(desc);
		if (GameManager::m_Pieces.size() ==
			AppClient::Instance()->BH_WaitPiecesFinish->TargetPieceNum)
		{
			Client_NotifyCountPlayerFinished();
		}
		break;
	}

	case MsgContent::CLIENT_LOAD_GAMESCENE:
	{
		AppClient::Instance()->ChangeToGameScene();
		break;
	}

	case MsgContent::CLIENT_SHOW_PIECE_ENTRY:
	{
		//start wait once
		AppClient::Instance()->StartWaitPiecesFinish();

		//test
		Data_Int4* pieceEntryData = (Data_Int4*)msgContent.Data;
		int pieceID = pieceEntryData->Data[0];
		int squareID = pieceEntryData->Data[1];
		AppClient::Instance()->StartPieceShowEntry(pieceID, squareID);

		break;
	}

	case MsgContent::CLIENT_SHOW_STEP:
	{
		Data_Int4* stepTypeData = (Data_Int4*)msgContent.Data;
		int stepType = stepTypeData->Data[0];
		if (stepType == (int)Judgement::ShowStepType::TURN_START)
		{
			AppClient::Instance()->m_TurnCount = stepTypeData->Data[1];
		}
		AppClient::Instance()->StartWaitShowStep(stepType);

		break;
	}

	case MsgContent::CLIENT_INPUT_RPS:
	{
		AppClient::Instance()->ShowPiecesThinkMark();
		AppClient::Instance()->m_MainPlayer->StartIterateSelfPiecesInputHand();

		break;
	}

	case MsgContent::CLIENT_SET_PIECE_HAND:
	{
		Data_Int4* data = (Data_Int4*)msgContent.Data;
		int pieceID = data->Data[0];
		HandType handType = (HandType)data->Data[1];
		Piece* piece = GameManager::m_Pieces[pieceID];
		{
			//save handtype data in client for check actpoint show
			piece->FinishSetHand(handType);
		}

		break;
	}

	case MsgContent::CLIENT_SHOW_PIECE_HAND:
	{
		//start wait once
		AppClient::Instance()->StartWaitPiecesFinish();

		//test
		Data_Int* pieceIDData = (Data_Int*)msgContent.Data;
		int pieceID = pieceIDData->Data;
		AppClient::Instance()->StartPieceShowHand(pieceID);

		break;
	}

	case MsgContent::CLIENT_SHOW_CHECK_ACTPOINT:
	{
		//DebugInfo::Print("[get] show check piece actpoint");

		//test
		Data_Int4* data = (Data_Int4*)msgContent.Data;
		int pieceID = data->Data[0];
		int actPoint = data->Data[1];
		AppClient::Instance()->StartPieceShowCheckActpoint(pieceID, actPoint);

		break;
	}

	case MsgContent::CLIENT_INPUT_PIECE_ACT:
	{
		//DebugInfo::Print("[get] piece input act");

		//test
		Data_Int* pieceIDData = (Data_Int*)msgContent.Data;
		int pieceID = pieceIDData->Data;
		AppClient::Instance()->StartPieceInputAct(pieceID);

		break;
	}

	case MsgContent::CLIENT_SHOW_PIECE_FINISH:
	{
		//DebugInfo::Print("[get] piece finish act");

		//test
		Data_Int* pieceIDData = (Data_Int*)msgContent.Data;
		int pieceID = pieceIDData->Data;
		AppClient::Instance()->StartPieceShowFinishAct(pieceID);

		break;
	}

	case MsgContent::CLIENT_CLEAR_PIECE_HAND_AND_ACTPOINT:
	{
		AppClient::Instance()->StartWaitPiecesFinish();
		AppClient::Instance()->StartPiecesClearHandAndActpoint();

		break;
	}


	case MsgContent::CLIENT_SHOW_PIECE_MOVE:
	{
		Data_Int4* moveData = (Data_Int4*)msgContent.Data;
		int pieceID = moveData->Data[0];
		int squareID = moveData->Data[1];
		AppClient::Instance()->StartPieceShowMove(pieceID, squareID);

		break;
	}

	case MsgContent::CLIENT_CONTINUE_INPUT_PIECE_MOVE:
	{
		Data_Int* moveData = (Data_Int*)msgContent.Data;
		int pieceID = moveData->Data;

		Piece* piece = GameManager::m_Pieces[pieceID];
		piece->StartInputMove();

		break;
	}

	case MsgContent::CLIENT_SHOW_PIECE_CAUGHT:
	{
		Data_Int4* caughtData = (Data_Int4*)msgContent.Data;
		int movePieceID = caughtData->Data[0];
		int caughtPieceID = caughtData->Data[1];
		int prisonRoomSquareID = caughtData->Data[2];
		AppClient::Instance()->StartPieceShowCaught(movePieceID, caughtPieceID, prisonRoomSquareID);

		break;
	}

	case MsgContent::CLIENT_SHOW_PIECE_ESCAPE:
	{
		Data_Int4* escapeData = (Data_Int4*)msgContent.Data;
		int escapePieceID = escapeData->Data[0];
		int escapeSquareID = escapeData->Data[1];
		AppClient::Instance()->StartPieceShowEscape(escapePieceID, escapeSquareID);

		break;
	}

	case MsgContent::CLIENT_SHOW_GAME_OVER:
	{
		Data_Int* gameOverData = (Data_Int*)msgContent.Data;
		int result = gameOverData->Data;
		AppClient::Instance()->StartShowGameOver(result);

		break;
	}
	}
}

#pragma region ========== send func ==========
void NetworkSendFunc::Client_SendTestMsgToBroadcast(const char* sentence)
{
	//data for serilize
	Data_Char256 data;
	strcpy_s(data.Sentence, sentence);
	MsgContent content = { MsgContent::TEST_SERVER_BROADCAST,
		LEN_DATA_CHAR256, &data };

	//add to sendmsgqueue, send by tcp
	GetMyClient()->SendToServer(content, "send chat msg ");
}
void NetworkSendFunc::Server_BroadcastTestMsgToShow(const char* sentence)
{
	//data for serilize
	Data_Char256 data;
	memcpy(data.Sentence, sentence, LEN_DATA_CHAR256);
	MsgContent content = { MsgContent::TEST_SHOW_MSG,
		LEN_DATA_CHAR256,&data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content, "broadcast chat msg ");
}

void NetworkSendFunc::Client_RequestDisconnect()
{
	//data for serilize
	Data_Int data;
	data.Data = GetMyClient()->m_ClientID;
	MsgContent content = { MsgContent::SERVER_SET_CLIENT_DISCONNECT,
		LEN_DATA_INT, &data };

	//add to sendmsgqueue, send by tcp
	GetMyClient()->SendDisconnect(EncodeMsgContent(content));
}
void NetworkSendFunc::Server_CommandDisconnect()
{
	//data for serilize
	MsgContent content = { MsgContent::CLIENT_DISCONNECT,
		LEN_ZERO, nullptr };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content, "command server close ");
}

void NetworkSendFunc::Client_RequestJoinServer(AppClient* appClient)
{
	//data for serilize
	Data_Client data;
	memcpy(data.Name, appClient->m_Name, LEN_CLIENT_NAME);
	memcpy(data.IP, GetMyClient()->m_IP.c_str(), 64);
	data.Ready = false;//default
	data.ID = appClient->m_MyClient->m_ClientID;//default
	MsgContent content = { MsgContent::SERVER_CHECK_CLIENT_JOIN,
		LEN_DATA_CLIENT, &data };

	//add to sendmsgqueue, send by tcp
	GetMyClient()->SendToServer(content, "request join server ");
}
void NetworkSendFunc::Client_TryConnectServer()
{
	//tcp three hand shake
	DebugInfo::Print("try connect server...");
	GetMyClient()->TryConnect();

#if 0
	//WSAAsyncSelect
	int nRtn = WSAAsyncSelect(Socket, GetWindow(), NET_MSG,
		FD_CONNECT | FD_WRITE | FD_READ | FD_CLOSE);
	if (nRtn == SOCKET_ERROR)
	{
		int nError = WSAGetLastError();
		if (nError != WSAEWOULDBLOCK)
		{
			MessageBox(GetWindow(), "非同期化失敗", WINDOW_NAME, MB_OK);
			DebugInfo::Print("async select failed");
			closesocket(Socket);
			return;
		}
	}
	else
	{
		DebugInfo::Print("async select success");
	}

	// サーバーへ接続するための準備
	SOCKADDR_IN addrin;
	memset(&addrin, 0, LEN_ADDRIN);
	addrin.sin_family = AF_INET;
	addrin.sin_port = htons(Port);
	addrin.sin_addr.s_addr = *((unsigned long*)HostEnt->h_addr);

	//start non-blocking, need back to blocking in FD_CONNECT
	//u_long nonBlock = 1;
	//ioctlsocket(Socket, FIONBIO, &nonBlock);
	nRtn = connect(Socket, (SOCKADDR*)&addrin, LEN_ADDRIN);
	if (nRtn < 0)
	{
		DebugInfo::Print("first connect server failed");
	}
	else
	{
		DebugInfo::Print("connect...");
	}
#endif
}
void NetworkSendFunc::Server_CommandClientSetID(int setClientID)
{
	//data for serilize
	Data_Int data;
	data.Data = setClientID;//client id
	MsgContent content = { MsgContent::CLIENT_SET_ID,
		LEN_DATA_INT, &data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToOne(setClientID, content, "command set id ");
}
void NetworkSendFunc::Server_AgreeJoin(int setClientID, int targetClientNum)
{
	//data for serilize
	Data_Int4 data;
	data.Data[0] = setClientID;//client id
	data.Data[1] = targetClientNum;//target client num
	MsgContent content = { MsgContent::CLIENT_JOIN_ROOM,
		LEN_DATA_INT4, &data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToOne(setClientID, content, "response agree join ");
}
void NetworkSendFunc::Server_DisagreeJoin(int setClientID)
{
	//data for serilize
	Data_Int data;
	data.Data = setClientID;//client id for auto disconnect
	MsgContent content = { MsgContent::CLIENT_JOIN_ROOM_FAIL,
		LEN_DATA_INT, &data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToOne(setClientID, content, "response agree join ");
}
void NetworkSendFunc::Server_SendJoinedClientInfosToClient(int clientID, AppServer* appServer)
{
	for (auto idMem : appServer->m_ClientMembers)
	{
		//data for serilize
		Data_Client data;
		ClientMember& mem = *idMem.second;
		memcpy(data.Name, mem.Name.c_str(), LEN_CLIENT_NAME);
		data.ID = mem.ID;
		data.Ready = mem.Ready;
		//data.IP = //for private chat
		MsgContent content = { MsgContent::CLIENT_UPDATE_JOINEDNUM,
			LEN_DATA_CLIENT, &data };

		//add to sendmsgqueue, send by tcp
		GetMyServer()->SendToOne(clientID, content,
			"send info of other member[" + std::to_string(mem.ID) + "] ");
	}
}
void NetworkSendFunc::Server_CommandUpdateJoinedNum(ClientMember* member)
{
	//data for serilize
	Data_Client data;
	//data0:joinedNum,data1:targetNum
	memcpy(data.Name, member->Name.c_str(), LEN_CLIENT_NAME);
	//todo
	//memcpy(data.IP, inet_ntoa(member->UDPAddr->sin_addr), 64);
	data.ID = member->ID;
	data.Ready = member->Ready;
	data.Disconnect = member->Disconnect;
	MsgContent content = { MsgContent::CLIENT_UPDATE_JOINEDNUM,
		LEN_DATA_CLIENT,&data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content, "command update client member info ");
}
void NetworkSendFunc::Client_RequestSetReady(int clientID, bool ready)
{
	//data for serilize
	Data_Int4 data;
	data.Data[0] = clientID;
	data.Data[1] = ready ? 1 : 0;//0:not ready, 1:ready
	MsgContent content = { MsgContent::SERVER_SET_CLIENT_READY,
		LEN_DATA_INT4,&data };

	//add to sendmsgqueue, send by tcp
	GetMyClient()->SendToServer(content,
		"request set ready[" + std::string(ready ? "true" : "false") + "] ");
}

void NetworkSendFunc::Judgement_CommandCreatePlayer()
{
	for (auto idPlayer : GameManager::m_Players)
	{
		Player* player = idPlayer.second;

		//data for serilize
		Data_PlayerDesc data{
			false,
			player->m_SelfPieces.GetList()->size(),
			player->m_Camp,
			player->m_ID
		};
		MsgContent content = { MsgContent::CLIENT_CREATE_PLAYER,
			LEN_DATA_PLAYERDESC,&data };

		//add to sendmsgqueue, send by tcp
		GetMyServer()->SendToAll(content, "command create self player ");
	}
}
void NetworkSendFunc::Judgement_CommandSetPieceNum()
{
	//data for serilize
	int pieceNum = GameManager::m_Pieces.size();
	Data_Int data{ pieceNum };
	MsgContent content = { MsgContent::CLIENT_SET_PIECE_NUM,
		LEN_DATA_INT,&data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content,
		"command set piece num " + std::to_string(pieceNum));
}
void NetworkSendFunc::Judgement_CommandCreatePiece(Piece* piece)
{
	//data for serilize
	Data_PieceDesc data{
		piece->m_CharaType,
		piece->m_Camp,
		piece->m_ID,
		piece->m_OwnerPlayer->m_ID
	};
	MsgContent content = { MsgContent::CLIENT_CREATE_PIECE,
		LEN_DATA_PIECEDESC,&data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content,
		"command create piece[" + std::to_string(piece->m_ID) + "] ");
}

void NetworkSendFunc::Server_CommandLoadGameScene()
{
	//data for serilize
	MsgContent content = { MsgContent::CLIENT_LOAD_GAMESCENE,
		LEN_ZERO,nullptr };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content, "command load game scene ");
}
void NetworkSendFunc::Judgement_CommandPieceShowEntry(Piece* piece)
{
	//data for serilize
	Data_Int4 data{ piece->m_ID, piece->m_FootOnSquare->m_ID };
	MsgContent content = { MsgContent::CLIENT_SHOW_PIECE_ENTRY,
		LEN_DATA_INT4,&data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content,
		"command show piece[" +
		std::to_string(piece->m_ID) +
		"] entry to square[" +
		std::to_string(piece->m_FootOnSquare->m_ID) +
		"] ");
}

void NetworkSendFunc::Judgement_CommandShowStep(int showStepType)
{
	//data for serilize
	Data_Int4 data;// { showStepType };
	data.Data[0] = showStepType;
	//when step turn start, need send turn count
	if (showStepType == (int)Judgement::ShowStepType::TURN_START)
	{
		data.Data[1] = Judgement::Instance()->m_TurnCount;
	}
	MsgContent content = { MsgContent::CLIENT_SHOW_STEP,
		LEN_DATA_INT4,&data };

	//for show info
	std::string stepName;
	switch ((Judgement::ShowStepType)showStepType)
	{
	case Judgement::ShowStepType::GAME_START:
		stepName = "[game start]";
		break;

	case Judgement::ShowStepType::TURN_START:
		stepName = "[turn start]";
		break;

	case Judgement::ShowStepType::RPS_START:
		stepName = "[rps start]";
		break;

	case Judgement::ShowStepType::ACT_START:
		stepName = "[act start]";
		break;

	case Judgement::ShowStepType::TURN_END:
		stepName = "[turn end]";
		break;
	}

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content, "command show step" + stepName);
}

void NetworkSendFunc::Judgement_CommandInputRPS()
{
	//data for serilize
	MsgContent content = { MsgContent::CLIENT_INPUT_RPS,
		LEN_ZERO,nullptr };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content, "command start input rps");
}
void NetworkSendFunc::Judgement_CommandShowPieceHand(Piece* piece)
{
	//data for serilize
	Data_Int data{ piece->m_ID };
	MsgContent content = { MsgContent::CLIENT_SHOW_PIECE_HAND,
		LEN_DATA_INT,&data };

	//add to sendmsgqueue, send by tcp
	//test
	//todo need change to BH call?
	GetMyServer()->SendToAll(content,
		"command show piece[" +
		std::to_string(piece->m_ID) +
		"] hand[" +
		Hand::HandName(*piece->m_Hands.begin()) +
		"]");
}
void NetworkSendFunc::Judgement_CommandPieceShowCheckActpoint(Piece* piece)
{
	//data for serilize
	Data_Int4 data{ piece->m_ID,piece->m_ActPoint };
	MsgContent content = { MsgContent::CLIENT_SHOW_CHECK_ACTPOINT,
		LEN_DATA_INT4,&data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content,
		"command piece[" +
		std::to_string(piece->m_ID) +
		"] show check actpoint");
}

void NetworkSendFunc::Client_NotifyCountPlayerFinished()
{
	//data for serilize
	MsgContent content = { MsgContent::JUDGEMENT_COUNT_PLAYER_FINISHED,
		LEN_ZERO, nullptr };

	//add to sendmsgqueue, send by tcp
	GetMyClient()->SendToServer(content, "notify self finish");
}

void NetworkSendFunc::Piece_RequestSetHand(int pieceID, HandType handType)
{
	//data for serilize
	Data_Int4 data;
	data.Data[0] = pieceID;
	data.Data[1] = (int)handType;
	MsgContent content = { MsgContent::JUDGEMENT_COMMAND_SET_HAND,
		LEN_DATA_INT4, &data };

	//add to sendmsgqueue, send by tcp
	GetMyClient()->SendToServer(content,
		"request set piece[" +
		std::to_string(pieceID) +
		"] hand[" +
		Hand::HandName(handType) +
		"] ");
}
void NetworkSendFunc::Judgement_CommandSetHand(int pieceID, HandType handType)
{
	//data for serilize
	Data_Int4 data;// { showStepType };
	data.Data[0] = pieceID;
	data.Data[1] = (int)handType;
	MsgContent content = { MsgContent::CLIENT_SET_PIECE_HAND,
		LEN_DATA_INT4,&data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content,
		"command add [" +
		Hand::HandName(handType) +
		"] to piece[" +
		std::to_string(pieceID) +
		"]");
}

void NetworkSendFunc::Judgement_CommandPieceInputAct(Piece* piece)
{
	//data for serilize
	Data_Int data{ piece->m_ID };
	MsgContent content = { MsgContent::CLIENT_INPUT_PIECE_ACT,
		LEN_DATA_INT,&data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content,
		"command piece[" +
		std::to_string(piece->m_ID) +
		"] input act");
}
void NetworkSendFunc::Piece_RequestFinishAct(int pieceID)
{
	//data for serilize
	Data_Int data;
	data.Data = pieceID;
	MsgContent content = { MsgContent::JUDGEMENT_COMMAND_PIECE_FINISH,
		LEN_DATA_INT, &data };

	//add to sendmsgqueue, send by tcp
	GetMyClient()->SendToServer(content,
		"request piece[" +
		std::to_string(pieceID) +
		"] finish act ");
}
void NetworkSendFunc::Judgement_CommandPieceFinishAct(int pieceID)
{
	//data for serilize
	Data_Int data{ pieceID };
	MsgContent content = { MsgContent::CLIENT_SHOW_PIECE_FINISH,
		LEN_DATA_INT,&data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content,
		"command piece[" +
		std::to_string(pieceID) +
		"] finish act ");
}
void NetworkSendFunc::Piece_RequestMoveAct(int pieceID, int squareID)
{
	//data for serilize
	Data_Int4 data;
	data.Data[0] = pieceID;
	data.Data[1] = squareID;
	MsgContent content = { MsgContent::JUDGEMENT_CHECK_PIECE_MOVE,
		LEN_DATA_INT4, &data };

	//add to sendmsgqueue, send by tcp
	GetMyClient()->SendToServer(content,
		"request piece[" +
		std::to_string(pieceID) +
		"] move to square[" +
		std::to_string(squareID) +
		"] ");
}
void NetworkSendFunc::Judgement_CommandPieceMove(int pieceID, int squareID)
{
	//data for serilize
	Data_Int4 data;
	data.Data[0] = pieceID;
	data.Data[1] = squareID;
	MsgContent content = { MsgContent::CLIENT_SHOW_PIECE_MOVE,
		LEN_DATA_INT4, &data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content,
		"command piece[" +
		std::to_string(pieceID) +
		"] move to square[" +
		std::to_string(squareID) +
		"]");
}
void NetworkSendFunc::Judgement_CommandPieceContinueMove(int clientID, int pieceID)
{
	//data for serilize
	Data_Int data;
	data.Data = pieceID;
	MsgContent content = { MsgContent::CLIENT_CONTINUE_INPUT_PIECE_MOVE,
		LEN_DATA_INT, &data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToOne(clientID, content,
		"command piece[" +
		std::to_string(pieceID) +
		"] continue move ");
}
void NetworkSendFunc::Judgement_CommandPieceCaught(int movePieceID, int caughtPieceID, int prisonRoomSquareID)
{
	//data for serilize
	Data_Int4 data;
	data.Data[0] = movePieceID;
	data.Data[1] = caughtPieceID;
	data.Data[2] = prisonRoomSquareID;
	MsgContent content = { MsgContent::CLIENT_SHOW_PIECE_CAUGHT,
		LEN_DATA_INT4, &data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content,
		"command piece[" +
		std::to_string(movePieceID) +
		"] catch piece[" +
		std::to_string(caughtPieceID) +
		"] ");
}
void NetworkSendFunc::Judgement_CommandPieceEscape(int escapePieceID, int goalSquareID)
{
	//data for serilize
	Data_Int4 data;
	data.Data[0] = escapePieceID;
	data.Data[1] = goalSquareID;
	MsgContent content = { MsgContent::CLIENT_SHOW_PIECE_ESCAPE,
		LEN_DATA_INT4, &data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content,
		"command piece[" +
		std::to_string(escapePieceID) +
		"] escape ");
}


void NetworkSendFunc::Judgement_CommandPiecesClearHandAndActpoint()
{
	//data for serilize
	MsgContent content = { MsgContent::CLIENT_CLEAR_PIECE_HAND_AND_ACTPOINT,
		LEN_ZERO,nullptr };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content, "command clear hand and actpoint ");
}
void NetworkSendFunc::Judgement_CommandPieceClearActpoint(Piece* piece)
{

}
void NetworkSendFunc::Judgement_CommandPieceClearHand(Piece* piece)
{

}
void NetworkSendFunc::Client_RequestUpdatePieceActpoint(Piece* piece)
{

}
void NetworkSendFunc::Client_RequestUpdatePieceHand(Piece* piece)
{

}
void NetworkSendFunc::Judgement_CommandShowGameOver(int result)
{
	//data for serilize
	Data_Int data{ result };
	MsgContent content = { MsgContent::CLIENT_SHOW_GAME_OVER,
		LEN_DATA_INT,&data };

	//add to sendmsgqueue, send by tcp
	GetMyServer()->SendToAll(content, "command show game over ");
}
#pragma endregion

NetworkSendFunc g_NetSendFunc;

NetworkSendFunc& GetNetSendFunc()
{
	return g_NetSendFunc;
}
