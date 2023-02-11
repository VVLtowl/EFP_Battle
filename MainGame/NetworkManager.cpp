#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define NET_MSG (WM_USER + 1)

//test
#define NET_TCPMSG1 (NET_MSG+1)
#define NET_TCPMSG2 (NET_MSG+2)
#define NET_TCPMSG3 (NET_MSG+3)
#define NET_TCPMSG4 (NET_MSG+4)
#define NET_TCPMSG5 (NET_MSG+5)
#define NET_TCPMSG6 (NET_MSG+6)

#include "main.h"
#include <string.h>

#include <windows.h>
#include <windowsx.h>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")

#include "Square.h"
#include "Board.h"
#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "Judgement.h"

#include "SerializedTool.h"

#include "NetworkManager.h"
#include "ServerBehaviour.h"
#include "ClientBehaviour.h"
#include "PieceBehaviour.h"
#include "JudgementBehaviour.h"
#include "Server.h"
#include "Client.h"

#include "imgui/imgui.h"

#pragma region ========== msg structure definition ==========

SendMsg::SendMsg(const char* msg)
{
	memcpy(MsgBuffer, msg, LEN_MSG);

	TCPSockID = -1;
	memset(&Destination, 0, LEN_ADDRIN);
}

SendMsg::SendMsg(int tcpSockID, const char* msg)
{
	TCPSockID = tcpSockID;
	memcpy(MsgBuffer, msg, LEN_MSG);

	memset(&Destination, 0, LEN_ADDRIN);
}

SendMsg::SendMsg(const SOCKADDR_IN& addrin, const char* msg)
{
	memcpy(&Destination, &addrin, LEN_ADDRIN);
	memcpy(MsgBuffer, msg, LEN_MSG);
}

RecvMsg::RecvMsg(const char* msg)
{
	memcpy(MsgBuffer, msg, LEN_MSG);

	//client side just use msgbuffer
	TCPSockID = -1;
	memset(&From, 0, LEN_ADDRIN);
}

RecvMsg::RecvMsg(const int tcpSockID, const char* msg)
{
	TCPSockID = tcpSockID;
	memcpy(MsgBuffer, msg, LEN_MSG);

	//useless in tcp
	memset(&From, 0, LEN_ADDRIN);
}

RecvMsg::RecvMsg(const SOCKADDR_IN& addrin, const char* msg)
{
	memcpy(&From, &addrin, LEN_ADDRIN);
	memcpy(MsgBuffer, msg, LEN_MSG);
}
#pragma endregion

#pragma region ========== start and close ==========

int NetworkManager::GetEnableTCPID()
{
	int id = 0;
	while (id < MAX_TCPSOCK)
	{
		if (TCPSockets[id].IsUsed == false)
		{
			TCPSockets[id].IsUsed = true;
			break;
		}
		else
		{
			id++;
		}
	}
	if (id > MAX_TCPSOCK)
	{
		id = -1;
	}
	return id;
}

void NetworkManager::StartAsServer()
{
	if (Running)
	{
		return;
	}

	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
	{
		MessageBox(NULL, "WSAStartup Error", WINDOW_NAME, MB_OK);
		DebugInfo::Print("start wsa server failed");
	}
	else
	{
		//tell mainproc need to listen net
		Running = true;

		//set capture sub name
		SetSubName("EFP [server]");

		//set server
		TargetServer = Server::Instance();

		DebugInfo::Print("start wsa server success");

		InitChatPanel();
	}
}

void NetworkManager::TryLaunchServer(int port,bool isTcp)
{
	if (!Running)
	{
		return;
	}

	//set tcp transport protocol
	IsTCP = isTcp;

	//make socket
	{
		//set port
		Port = port;

		//set HostEnt
		char hostName[256];
		gethostname(hostName, (int)sizeof(hostName));
		HostEnt = gethostbyname(hostName);

		//set serverIP
		IN_ADDR inaddr;
		memcpy(&inaddr, HostEnt->h_addr_list[0], 4);
		strcpy_s(HostIP, inet_ntoa(inaddr));

		if (IsTCP)
			//tcp
		{
			//set socket
			TCPListenSocket = socket(AF_INET, SOCK_STREAM, 0);
			if (TCPListenSocket < 0)
			{
				MessageBox(GetWindow(), "create ListenSocket failed", WINDOW_NAME, MB_OK);
				return;
			}
			else
			{
				DebugInfo::Print("create ListenSocket success");
			}

			//describe socketaddr
			SOCKADDR_IN socketaddr;
			memset(&socketaddr, 0, LEN_ADDRIN);
			socketaddr.sin_family = AF_INET;
			socketaddr.sin_port = htons(Port);
			socketaddr.sin_addr.s_addr = htonl(INADDR_ANY);

			//bind
			int nRtn = bind(TCPListenSocket, (LPSOCKADDR)&socketaddr, (int)sizeof(socketaddr));
			if (nRtn == SOCKET_ERROR)
			{
				MessageBox(GetWindow(), "ListenSocket bind failed", WINDOW_NAME, MB_OK);
				DebugInfo::Print("ListenSocket bind failed");
				closesocket(TCPListenSocket);
				return;
			}
			else
			{
				DebugInfo::Print("ListenSocket bind success");
			}

			//listen
			nRtn = listen(TCPListenSocket, 0);
			if (nRtn == SOCKET_ERROR)
			{
				MessageBox(GetWindow(), "ListenSocket listen failed", WINDOW_NAME, MB_OK);
				DebugInfo::Print("ListenSocket listen failed");
				closesocket(TCPListenSocket);
				return;
			}
			else 
			{
				DebugInfo::Print("ListenSocket listen success");
			}

			//WSAAsyncSelect
			nRtn = WSAAsyncSelect(TCPListenSocket, GetWindow(), NET_MSG, FD_ACCEPT);
			if (nRtn == SOCKET_ERROR)
			{
				int nError = WSAGetLastError();
				if (nError != WSAEWOULDBLOCK)
				{
					MessageBox(GetWindow(), "ListenSocket async failed", WINDOW_NAME, MB_OK);
					DebugInfo::Print("ListenSocket async failed");
					closesocket(Socket);
					return;
				}
			}
			else
			{
				DebugInfo::Print("ListenSocket async success");
			}
		}
		else
			//udp
		{
			//set socket
			Socket = socket(AF_INET, SOCK_DGRAM, 0);
			if (Socket < 0)
			{
				MessageBox(GetWindow(), "socket失敗", WINDOW_NAME, MB_OK);
				return;
			}

			//describe socketaddr
			SOCKADDR_IN socketaddr;
			memset(&socketaddr, 0, sizeof(socketaddr));
			socketaddr.sin_family = AF_INET;
			socketaddr.sin_port = htons(Port);
			socketaddr.sin_addr.s_addr = htonl(INADDR_ANY);

			//bind
			int nRtn = bind(Socket, (LPSOCKADDR)&socketaddr, (int)sizeof(socketaddr));
			if (nRtn == SOCKET_ERROR)
			{
				MessageBox(GetWindow(), "bindエラーです", WINDOW_NAME, MB_OK);
				closesocket(Socket);
				return;
			}

			//WSAAsyncSelect
			nRtn = WSAAsyncSelect(Socket, GetWindow(), NET_MSG, FD_READ | FD_WRITE);
			if (nRtn == SOCKET_ERROR)
			{
				int nError = WSAGetLastError();
				if (nError != WSAEWOULDBLOCK)
				{
					MessageBox(GetWindow(), "非同期化失敗", WINDOW_NAME, MB_OK);
					closesocket(Socket);
					return;
				}
			}
		}
	}
}

void NetworkManager::StartAsClient()
{
	if (Running)
	{
		return;
	}

	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
	{
		MessageBox(NULL, "WSAStartup Error", WINDOW_NAME, MB_OK);
		DebugInfo::Print("start wsa client failed");
	}
	else
	{
		//tell mainproc need to listen net
		Running = true;

		//set capture sub name
		SetSubName("EFP [client]");

		//set client
		TargetClient = Client::Instance();

		DebugInfo::Print("start wsa client success");

		InitChatPanel();
	}
}

void NetworkManager::SetConnectServerInfo(int port, std::string serverIP, bool isTcp)
{
	if (!Running)
	{
		return;
	}

	//set is tcp
	IsTCP = isTcp;

	//make socket
	if (IsTCP)
		//tcp
	{
		//set port
		Port = port;
		TargetClient->ServerPort = port;

		//set HostEnt
		unsigned int IP = inet_addr(serverIP.c_str());
		HostEnt = gethostbyaddr(serverIP.c_str(), 4, AF_INET);
		HostEnt = gethostbyname(serverIP.c_str());
		if (HostEnt == nullptr)
		{
			MessageBox(GetWindow(), "ホスト情報が取得できません", WINDOW_NAME, MB_OK);
			return;
		}

		//set serverIP
		IN_ADDR inaddr;
		memcpy(&inaddr, HostEnt->h_addr_list[0], 4);
		strcpy_s(HostIP, inet_ntoa(inaddr));

		//set socket
		Socket = socket(AF_INET, SOCK_STREAM, 0);
		if (Socket < 0)
		{
			MessageBox(GetWindow(), "socket失敗", WINDOW_NAME, MB_OK);
			return;
		}
	}
	else
		//udp
	{
		//set port
		Port = port;

		//set HostEnt
		HostEnt = gethostbyname(serverIP.c_str());

		//set serverIP
		IN_ADDR inaddr;
		memcpy(&inaddr, HostEnt->h_addr_list[0], 4);
		strcpy_s(HostIP, inet_ntoa(inaddr));

		//set socket
		Socket = socket(AF_INET, SOCK_DGRAM, 0);
		if (NetworkManager::Instance()->Socket < 0)
		{
			MessageBox(GetWindow(), "socket失敗", WINDOW_NAME, MB_OK);
			return;
		}

		//WSAAsyncSelect
		int nRtn = WSAAsyncSelect(Socket, GetWindow(), NET_MSG, FD_READ | FD_WRITE);
		if (nRtn == SOCKET_ERROR)
		{
			int nError = WSAGetLastError();
			if (nError != WSAEWOULDBLOCK)
			{
				MessageBox(GetWindow(), "非同期化失敗", WINDOW_NAME, MB_OK);
				closesocket(Socket);
				return;
			}
		}
	}
}


void NetworkManager::InitChatPanel()
{
	if (!Running)
	{
		return;
	}

	//test
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_NETWORK_CHAT, [this]()
		{
			if (TargetClient)
			{
				if (TargetClient->JoinSuccess)
				{
					DebugInfo::DrawNetMsg();
				}
			}

			if (TargetServer)
			{
				if (TargetServer->OpenGameRoom)
				{
					DebugInfo::DrawNetMsg();
				}
			}
		});
}

void NetworkManager::Close()
{
	if (!Running)
	{
		return;
	}

	//close chat panel
	DebugInfo::CloseBlock(TESTBLOCKID_NETWORK_CHAT);

	//disconnect
	if (TargetClient)
	{
		Client_RequestDisconnect();
	}

	if (TargetServer)
	{
		Server_CommandDisconnect();
	}

	//send last time
	Update();

	//wait for last send
	MessageBox(NULL, "クリーンアップします", WINDOW_NAME, MB_OK);

	//clean
	TargetClient = nullptr;
	TargetServer = nullptr;
	Running = false;
	closesocket(Socket);
	closesocket(TCPListenSocket);
	for (int i = 0; i < MAX_TCPSOCK; i++)
	{
		closesocket(TCPSockets[i].Socket);
	}

	if (WSACleanup() != 0)
	{
		MessageBox(NULL, "クリーンアップ失敗です", WINDOW_NAME, MB_OK);
	}
	else
	{
		MessageBox(NULL, "クリーンアップ success", WINDOW_NAME, MB_OK);
	}
}

#pragma endregion

#pragma region ========== send method ==========

void NetworkManager::Client_SendTestMsgToBroadcast(const char* sentence)
{
	if (TargetClient)
	{
		//data for serilize
		Data_Char256 data;
		strcpy_s(data.Sentence, sentence);
		MsgContent msg = { MsgContent::TEST_SERVER_BROADCAST,
			LEN_DATA_CHAR256, &data };

		//make sendmsg
		SendMsg* send = nullptr;
		if (IsTCP)
		{
			send = new SendMsg(EncodeMsgContent(msg));

			send->PrintRowID = DebugInfo::Print("send");
			send->Comment = "request broadcast test msg";
		}
		else
		{
			send = new SendMsg(
				TargetClient->ServerAddr,
				EncodeMsgContent(msg));
		}

		//add to sendmsgqueue
		SendMsgQueue.push(send);
	}
}
void NetworkManager::Server_BroadcastTestMsgToShow(const char* sentence)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Char256 data;
		memcpy(data.Sentence, sentence, LEN_DATA_CHAR256);
		MsgContent msg = { MsgContent::TEST_SHOW_MSG,
			LEN_DATA_CHAR256,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf, "chat msg");
	}
}

void NetworkManager::Client_RequestDisconnect()
{
	if (TargetClient)
	{
		if (!TargetClient->IsConnected)
		{
			return;
		}

		//data for serilize
		Data_Int data;
		data.Data = TargetClient->ID;
		MsgContent msg = { MsgContent::SERVER_SET_CLIENT_DISCONNECT,
			LEN_DATA_INT, &data };

		//make sendmsg
		SendMsg* send = nullptr;
		if (IsTCP)
		{
			send = new SendMsg(EncodeMsgContent(msg));
			send->PrintRowID = DebugInfo::Print("send");
			send->Comment = "request disconnect";
		}
		else
		{
			send = new SendMsg(
				TargetClient->ServerAddr,
				EncodeMsgContent(msg));
		}

		//add to sendmsgqueue
		SendMsgQueue.push(send);
	}
}
void NetworkManager::Server_CommandDisconnect()
{
	if (TargetServer)
	{
		//data for serilize
		MsgContent msg = { MsgContent::CLIENT_DISCONNECT,
			LEN_ZERO, nullptr };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf, "");
	}
}

void NetworkManager::Client_RequestJoinServer()
{
	if(TargetClient)
	{
		//data for serilize
		Data_Client data;
		memcpy(data.Name, TargetClient->Name, LEN_CLIENT_NAME);
		memcpy(data.IP, HostIP, 64);
		MsgContent msg = { MsgContent::SERVER_CHECK_CLIENT_JOIN,
			LEN_DATA_CLIENT, &data };

		//make sendmsg
		SendMsg* send = nullptr;
		if (IsTCP)
		{
			send = new SendMsg(EncodeMsgContent(msg));
			send->PrintRowID = DebugInfo::Print("send");
			send->Comment = "request join server";
		}
		else
		{
			send = new SendMsg(
				TargetClient->ServerAddr,
				EncodeMsgContent(msg));
		}

		//add to sendmsgqueue
		SendMsgQueue.push(send);
	}
}
void NetworkManager::Client_TryConnectServer()
{
	if (TargetClient)
	{
		if (IsTCP)
			//tcp
		{
			//tcp three hand shake
			DebugInfo::Print("try connect server...");

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

			nRtn = connect(Socket, (SOCKADDR*)&addrin, LEN_ADDRIN);
			if (nRtn == SOCKET_ERROR)
			{
				//int error = WSAGetLastError();
				//MessageBox(GetWindow(), "connect server 接続エラー", WINDOW_NAME, MB_OK);
				//DebugInfo::Print("try connect server failed");
				//closesocket(Socket);
				//return;
			}
			else
			{
				//DebugInfo::Print("try connect server succeed");
			}

			//connect(Socket, (SOCKADDR*)&TargetClient->ServerAddr,LEN_ADDRIN);
		}
	}
}
void NetworkManager::Server_AgreeJoin(ClientMember* member)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int data;
		data.Data = member->ID;
		MsgContent msg = { MsgContent::CLIENT_JOIN_ROOM,
			LEN_DATA_INT, &data };

		//make sendmsg
		SendMsg* send = nullptr;
		if (IsTCP)
		{
			send = new SendMsg(
				member->TCPSocketID,
				EncodeMsgContent(msg));

			send->PrintRowID = DebugInfo::Print("send");
			send->Comment=
				"to client" +
				std::string("[" + std::to_string(member->TCPSocketID) + "] ") +
				"agree join";
		}
		else
		{
			send = new SendMsg(
				member->Address,
				EncodeMsgContent(msg));
		}

		//add to sendmsgqueue
		SendMsgQueue.push(send);
	}
}
void NetworkManager::Server_CommandUpdateJoinedNum()
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int4 data;
		//data0:joinedNum,data1:targetNum
		data.Data[0] = TargetServer->JoinedClientNum;
		data.Data[1] = TargetServer->TargetClientNum;
		MsgContent msg = { MsgContent::CLIENT_UPDATE_JOINEDNUM,
			LEN_DATA_INT4,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf,"command update joined num");
	}
}

void NetworkManager::Judgement_CommandCreatePlayer()
{
	if (TargetServer)
	{
		for (auto idPlayer : Judgement::Instance()->Players)
		{
			Player* player = idPlayer.second;

			//data for serilize
			Data_PlayerDesc data{
				false,
				player->SelfPieces.GetList()->size(),
				player->Camp,
				player->ID
			};
			MsgContent msg = { MsgContent::CLIENT_CREATE_PLAYER,
				LEN_DATA_PLAYERDESC,&data };

			//make sendmsg
			ClientMember* member = TargetServer->ClientMembers[player->ID];
			SendMsg* send = nullptr;
			if (IsTCP)
			{
				send = new SendMsg(
					member->TCPSocketID,
					EncodeMsgContent(msg)); 

				send->PrintRowID = DebugInfo::Print("send");
				send->Comment =
					"to client" +
					std::string("[" + std::to_string(member->TCPSocketID) + "] ") +
					"command create player";
			}
			else
			{
				send = new SendMsg(
					member->Address,
					EncodeMsgContent(msg));
			}

			//add to sendmsgqueue
			SendMsgQueue.push(send);
		}
	}
}
void NetworkManager::Judgement_CommandSetPieceNum()
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int data{ Judgement::Instance()->Pieces.size() };
		MsgContent msg = { MsgContent::CLIENT_SET_PIECE_NUM,
			LEN_DATA_INT,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf, "set piece num " + std::to_string(data.Data));
	}
}
void NetworkManager::Judgement_CommandCreatePiece(Piece* piece)
{
	if (TargetServer)
	{
		//data for serilize
		Data_PieceDesc data{
			piece->CharaType,
			piece->Camp,
			piece->ID,
			piece->OwnerPlayer->ID
		};
		MsgContent msg = { MsgContent::CLIENT_CREATE_PIECE,
			LEN_DATA_PIECEDESC,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf, "command create piece " + std::to_string(piece->ID));
	}
}

void NetworkManager::Server_CommandLoadGameScene()
{
	if (TargetServer)
	{
		//data for serilize
		MsgContent msg = { MsgContent::CLIENT_LOAD_GAMESCENE,
			LEN_ZERO,nullptr };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf, "command load game scene");
	}
}
void NetworkManager::Judgement_CommandPieceShowEntry(Piece* piece)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int4 data{ piece->ID, piece->FootOnSquare->ID };
		MsgContent msg = { MsgContent::CLIENT_SHOW_PIECE_ENTRY,
			LEN_DATA_INT4,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf,
			"command show piece " +
			std::to_string(piece->ID) +
			" entry");
	}
}

void NetworkManager::Judgement_CommandShowStep(int showStepType)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int4 data;// { showStepType };
		data.Data[0] = showStepType;
		if (showStepType == (int)Judgement::ShowStepType::TURN_START)
		{
			data.Data[1] = Judgement::Instance()->TurnCount;
		}
		MsgContent msg = { MsgContent::CLIENT_SHOW_STEP,
			LEN_DATA_INT4,&data };
		char* msgBuf = EncodeMsgContent(msg);

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

		SentToAllClients(msgBuf,
			"command show step " +
			stepName);
	}
}

void NetworkManager::Judgement_CommandInputRPS()
{
	if (TargetServer)
	{
		//data for serilize
		MsgContent msg = { MsgContent::CLIENT_INPUT_RPS,
			LEN_ZERO,nullptr };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf,
			"command input rps ");
	}
}
void NetworkManager::Judgement_CommandShowPieceHand(Piece* piece)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int data{ piece->ID };
		MsgContent msg = { MsgContent::CLIENT_SHOW_PIECE_HAND,
			LEN_DATA_INT,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf,
			"command show piece " +
			std::to_string(piece->ID) +
			" hand[" +
			Hand::HandName(*piece->Hands.begin()) +
			"]");
	}
}
void NetworkManager::Judgement_CommandPieceShowCheckActpoint(Piece* piece)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int data{ piece->ID };
		MsgContent msg = { MsgContent::CLIENT_SHOW_CHECK_ACTPOINT,
			LEN_DATA_INT,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf,
			"command piece " +
			std::to_string(piece->ID) +
			" show check actpoint");
	}
}

void NetworkManager::Client_NotifyCountPlayerFinished()
{
	if (TargetClient)
	{
		//data for serilize
		MsgContent msg = { MsgContent::JUDGEMENT_COUNT_PLAYER_FINISHED,
			LEN_ZERO, nullptr };

		//make sendmsg
		SendMsg* send = nullptr;
		if (IsTCP)
		{
			send = new SendMsg(EncodeMsgContent(msg));
			send->PrintRowID = DebugInfo::Print("send");
			DebugInfo::Print("");
			send->Comment = "notify player finish";
		}
		else
		{
			send = new SendMsg(
				TargetClient->ServerAddr,
				EncodeMsgContent(msg));
		}

		//add to sendmsgqueue
		SendMsgQueue.push(send);
	}
}
void NetworkManager::Client_NotifyCountPieceFinished()
{
	if (TargetClient)
	{
		//data for serilize
		MsgContent msg = { MsgContent::JUDGEMENT_COUNT_PIECE_FINISHED,
			LEN_ZERO, nullptr };

		//make sendmsg
		SendMsg* send = nullptr;
		if (IsTCP)
		{
			send = new SendMsg(EncodeMsgContent(msg));
			send->PrintRowID = DebugInfo::Print("send");
			send->Comment = "notify piece finish";
		}
		else
		{
			send = new SendMsg(
				TargetClient->ServerAddr,
				EncodeMsgContent(msg));
		}

		//add to sendmsgqueue
		SendMsgQueue.push(send);
	}
}//wrong !!!!

void NetworkManager::Piece_RequestSetHand(int pieceID, HandType handType)
{
	if (TargetClient)
	{
		//data for serilize
		Data_Int4 data;
		data.Data[0] = pieceID;
		data.Data[1] = (int)handType;
		MsgContent msg = { MsgContent::JUDGEMENT_COMMAND_SET_HAND,
			LEN_DATA_INT4, &data };

		//make sendmsg
		SendMsg* send = nullptr;
		if (IsTCP)
		{
			send = new SendMsg(EncodeMsgContent(msg));
			send->PrintRowID = DebugInfo::Print("send");
			send->Comment = "request set piece hand";
		}
		else
		{
			send = new SendMsg(
				TargetClient->ServerAddr,
				EncodeMsgContent(msg));
		}

		//add to sendmsgqueue
		SendMsgQueue.push(send);
	}
}
void NetworkManager::Judgement_CommandSetHand(int pieceID, HandType handType)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int4 data;// { showStepType };
		data.Data[0] = pieceID;
		data.Data[1] = (int)handType;
		MsgContent msg = { MsgContent::CLIENT_SET_PIECE_HAND,
			LEN_DATA_INT4,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf,
			"command add [" +
			std::string(Hand::HandName(handType)) +
			"] to piece[" +
			std::to_string(pieceID) +
			"]");
	}
}

void NetworkManager::Judgement_CommandPieceInputAct(Piece* piece)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int data{ piece->ID };
		MsgContent msg = { MsgContent::CLIENT_INPUT_PIECE_ACT,
			LEN_DATA_INT,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf,
			"command piece "+
			std::to_string(piece->ID) + 
			" input act");
	}
}
void NetworkManager::Piece_RequestFinishAct(int pieceID)
{
	if (TargetClient)
	{
		//data for serilize
		Data_Int data;
		data.Data = pieceID;
		MsgContent msg = { MsgContent::JUDGEMENT_COMMAND_PIECE_FINISH,
			LEN_DATA_INT, &data };

		//make sendmsg
		SendMsg* send = nullptr;
		if (IsTCP)
		{
			send = new SendMsg(EncodeMsgContent(msg));
			send->PrintRowID = DebugInfo::Print("send");
			send->Comment = "request piece finish act";
		}
		else
		{
			send = new SendMsg(
				TargetClient->ServerAddr,
				EncodeMsgContent(msg));
		}

		//add to sendmsgqueue
		SendMsgQueue.push(send);
	}
}
void NetworkManager::Judgement_CommandPieceFinishAct(int pieceID)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int data{ pieceID };
		MsgContent msg = { MsgContent::CLIENT_SHOW_PIECE_FINISH,
			LEN_DATA_INT,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf, 
			"command piece " +
			std::to_string(pieceID) +
			" finish act");
	}
}
void NetworkManager::Piece_RequestMoveAct(int pieceID, int squareID)
{
	if (TargetClient)
	{
		//data for serilize
		Data_Int4 data;
		data.Data[0] = pieceID;
		data.Data[1] = squareID;
		MsgContent msg = { MsgContent::JUDGEMENT_CHECK_PIECE_MOVE,
			LEN_DATA_INT4, &data };

		//make sendmsg
		SendMsg* send = nullptr;
		if (IsTCP)
		{
			send = new SendMsg(EncodeMsgContent(msg));
			send->PrintRowID = DebugInfo::Print("send");
			send->Comment = "request piece move";
		}
		else
		{
			send = new SendMsg(
				TargetClient->ServerAddr,
				EncodeMsgContent(msg));
		}

		//add to sendmsgqueue
		SendMsgQueue.push(send);
	}
}
void NetworkManager::Judgement_CommandPieceMove(int pieceID, int squareID)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int4 data;
		data.Data[0] = pieceID;
		data.Data[1] = squareID;
		MsgContent msg = { MsgContent::CLIENT_SHOW_PIECE_MOVE,
			LEN_DATA_INT4, &data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf,
			"command piece " +
			std::to_string(pieceID) +
			" move");
	}
}

void NetworkManager::Judgement_CommandPiecesClearHandAndActpoint()
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int data{ Judgement::Instance()->Pieces.size() };
		MsgContent msg = { MsgContent::CLIENT_CLEAR_PIECE_HAND_AND_ACTPOINT,
			LEN_ZERO,nullptr };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf, "clear hand and actpoint ");
	}
}
#pragma endregion

#pragma region ========== update recv and send ==========
void NetworkManager::UpdateNetProc(UINT uMsg, LPARAM lp)
{
	//check recv
	if(uMsg == NET_MSG)
	{
		int ievent = WSAGETSELECTEVENT(lp);
		switch (ievent)
		{
		case FD_READ:
		{
			if (IsTCP)
				//tcp
				//tip: just for client (server's socket is listensocket)
			{
				int fromLen = sizeof(SOCKADDR);
				char msgBuf[LEN_MSG];

				//recv msg
				int nRtn = recv(
					Socket,
					msgBuf,
					LEN_MSG,
					0);
				{
					//check error
					if (nRtn == SOCKET_ERROR)
					{
						MessageBox(GetWindow(), "受信エラーです", WINDOW_NAME, MB_OK);
						DebugInfo::Print("tcp recv failed");
					}
					else
					{
						//DebugInfo::Print("tcp recv success");

						RecvMsg* recvMsg = new RecvMsg(msgBuf);

						RecvMsgQueue.push(recvMsg);
					}
				}
			}
			if (!IsTCP)
				//udp
			{
				SOCKADDR_IN from;
				int fromLen = sizeof(SOCKADDR);
				char msgBuf[LEN_MSG];

				//recv msg
				int nRtn = recvfrom(
					Socket,
					msgBuf,
					LEN_MSG,
					0,
					(SOCKADDR*)&from,
					&fromLen);
				{
					//check error
					if (nRtn == SOCKET_ERROR)
					{
						MessageBox(GetWindow(), "受信エラーです", WINDOW_NAME, MB_OK);
						DebugInfo::Print("udp recv failed");
					}
					else
					{
						DebugInfo::Print("udp recv success");

						RecvMsg* recv = new RecvMsg(from, msgBuf);

						RecvMsgQueue.push(recv);
					}
				}
			}
			break;
		}

		//just for tcp
		//server
		case FD_ACCEPT:
		{

			DebugInfo::Print("has accept client socket...");

			SOCKADDR_IN from;
			int fromLen = sizeof(SOCKADDR);

			int id = GetEnableTCPID();



			//accept
			TCPSockets[id].Socket = accept(TCPListenSocket, (SOCKADDR*)&from, &fromLen);
			if (TCPSockets[id].Socket == INVALID_SOCKET)
			{
				MessageBox(GetWindow(), "accept failed", WINDOW_NAME, MB_OK);
				TCPSockets[id].IsUsed = false;
				closesocket(TCPSockets[id].Socket);
				return;
			}
			else
			{
				DebugInfo::Print("succeed accept client socket" + std::to_string(id));

				//WSAAsyncSelect
				//test
				int nRtn = WSAAsyncSelect(TCPSockets[id].Socket, GetWindow(),
					(NET_MSG + id + 1), FD_READ | FD_WRITE);
				if (nRtn == SOCKET_ERROR)
				{
					int nError = WSAGetLastError();
					if (nError != WSAEWOULDBLOCK)
					{
						MessageBox(GetWindow(), "非同期化失敗", WINDOW_NAME, MB_OK);
						closesocket(Socket);
						return;
					}
				}
			}
			break;
		}

		//just for tcp
		//client
		case FD_CONNECT:
		{
			{
				if (TargetClient)
				{
					DebugInfo::Print("client to server connect finished");

					//set connected success
					TargetClient->IsConnected = true;
					Client_RequestJoinServer();
				}
			}
		}
		}
		return;
	}

	//server check tcp recv
	if (TargetServer)
	{
		if (uMsg - NET_MSG <= MAX_TCPSOCK)
		{
			int id = uMsg - NET_MSG - 1;
			TCPMsgRecv(id, lp);
			return;
		}
	}
}
void NetworkManager::Update()
{
	//check recv
	while (!RecvMsgQueue.empty())
	{
		RecvMsg* recv = RecvMsgQueue.front();
		{
			MsgContent msg;
			DecodeMsgContent(recv->MsgBuffer, msg);

			switch (msg.BHID)
			{
			case MsgContent::NONE:
			{
				break;
			}

#pragma region ========== server to do ==========

			case MsgContent::TEST_SERVER_BROADCAST:
			{
				Data_Char256* data = (Data_Char256*)msg.Data;
				std::string info = "";
				info.append(data->Sentence);
				DebugInfo::PrintNetMsg(info);

				Server_BroadcastTestMsgToShow(data->Sentence);

				break;
			}

			case MsgContent::SERVER_CHECK_CLIENT_JOIN:
				//just for udp
			{
				Data_Char256* clientNameData = (Data_Char256*)msg.Data;
				std::string name(clientNameData->Sentence);

				if (IsTCP)
				{
					TargetServer->JumpToBH(new ServerCheckClientJoin(
						TargetServer, recv->TCPSockID, name));
				}
				else
				{
					TargetServer->JumpToBH(new ServerCheckClientJoin(
						TargetServer, recv->From, name));
				}

				break;
			}

			case MsgContent::SERVER_SET_CLIENT_DISCONNECT:
			{
				Data_Int* clientIDData = (Data_Int*)msg.Data;
				int id = clientIDData->Data;
				ClientMember* disconnectClient = TargetServer->ClientMembers[id];

				//set quit info before quit
				std::string info =
					"- " +
					std::string("[" + std::to_string(disconnectClient->ID) + "]") +
					disconnectClient->Name +
					" quit room";
				DebugInfo::PrintNetMsg(info.c_str());

				//close tcp sock
				if (IsTCP)
				{
					int tcpSockID = disconnectClient->TCPSocketID;
					TCPSockets[tcpSockID].IsUsed = false;
					closesocket(TCPSockets[tcpSockID].Socket);
				}

				//update client members and notify other clients
				TargetServer->QuitClientMember(id);
				Server_CommandUpdateJoinedNum();
				Server_BroadcastTestMsgToShow(info.c_str());
				
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
				Data_Int4* data = (Data_Int4*)msg.Data;
				int pieceID = data->Data[0];
				HandType handType = (HandType)data->Data[1];

				//save handtype data in judgement
				Judgement::Instance()->Pieces[pieceID]->AddHand(handType);

				//broadcast to client
				Judgement_CommandSetHand(pieceID, handType);

				break;
			}

			case MsgContent::JUDGEMENT_CHECK_PIECE_MOVE:
			{
				Data_Int4* data = (Data_Int4*)msg.Data;
				int pieceID = data->Data[0];
				int squareID = data->Data[1];


				//change piece and square data in judgement at first
				Piece* piece = Judgement::Instance()->Pieces[pieceID];
				Square* oldSqr = piece->FootOnSquare;
				Square* newSqr = Board::Instance()->Squares[squareID];
				{
					//piece event: out square
					piece->FootOnSquare = nullptr;

					//square event: piece out 
					oldSqr->Piece = nullptr;

					//piece event: in square
					piece->FootOnSquare = newSqr;

					//square event: piece in
					newSqr->Piece = piece;
				}		

				//todo
				//check move event!!!

				//broadcast to client
				Judgement_CommandPieceMove(pieceID, squareID);

				break;
			}

			case MsgContent::JUDGEMENT_COMMAND_PIECE_FINISH:
			{
				Data_Int* data = (Data_Int*)msg.Data;
				int pieceID = data->Data;

				Judgement::Instance()->StartWaitClientsPieceFinishAct(pieceID);

				break;
			}

#pragma endregion

#pragma region ========== client to do ==========

			case MsgContent::TEST_SHOW_MSG:
			{
				Data_Char256* data = (Data_Char256*)msg.Data;
				std::string info = "";
				info.append(data->Sentence);
				DebugInfo::PrintNetMsg(info);

				break;
			}

			case MsgContent::CLIENT_JOIN_ROOM:
			{
				Data_Int* clientIDData = (Data_Int*)msg.Data;
				int id = clientIDData->Data;
				TargetClient->JoinRoomAndSetID(id);

				break;
			}

			case MsgContent::CLIENT_UPDATE_JOINEDNUM:
			{
				Data_Int4* memberNumberData = (Data_Int4*)msg.Data;
				int joinedNum = memberNumberData->Data[0];
				int targetNum = memberNumberData->Data[1];
				TargetClient->UpdateNum(joinedNum, targetNum);

				break;
			}

			case MsgContent::CLIENT_DISCONNECT:
			{
				TargetClient->Disconnect();
				DebugInfo::Print("server closed");

				//todo

				break;
			}

			case MsgContent::CLIENT_CREATE_PLAYER:
			{
				Data_PlayerDesc* playerDescData = (Data_PlayerDesc*)msg.Data;
				PlayerDesc desc = *playerDescData;
				TargetClient->CreatePlayer(desc);
				Client_NotifyCountPlayerFinished();
				break;
			}

			case MsgContent::CLIENT_SET_PIECE_NUM:
			{
				Data_Int* pieceNumData = (Data_Int*)msg.Data;
				TargetClient->BH_WaitPiecesFinish->TargetPieceNum =
					pieceNumData->Data;
				DebugInfo::Print(
					"set target piece num: " +
					std::to_string(pieceNumData->Data));
				Client_NotifyCountPlayerFinished();
				break;
			}

			case MsgContent::CLIENT_CREATE_PIECE:
			{
				Data_PieceDesc* pieceDescData = (Data_PieceDesc*)msg.Data;
				PieceDesc desc = *pieceDescData;
				TargetClient->CreatePiece(desc);
				if (TargetClient->Pieces.size() == 
					TargetClient->BH_WaitPiecesFinish->TargetPieceNum)
				{
					Client_NotifyCountPlayerFinished();
				}
				break;
			}

			case MsgContent::CLIENT_LOAD_GAMESCENE:
			{
				TargetClient->ChangeToGameScene();
				break;
			}

			case MsgContent::CLIENT_SHOW_PIECE_ENTRY:
			{
				//start wait once
				TargetClient->StartWaitPiecesFinish();

				//test
				Data_Int4* pieceEntryData = (Data_Int4*)msg.Data;
				int pieceID = pieceEntryData->Data[0];
				int squareID = pieceEntryData->Data[1];
				TargetClient->StartPieceShowEntry(pieceID, squareID);

				break;
			}

			case MsgContent::CLIENT_SHOW_STEP:
			{
				Data_Int4* stepTypeData = (Data_Int4*)msg.Data;
				int stepType = stepTypeData->Data[0];
				if (stepType == (int)Judgement::ShowStepType::TURN_START)
				{
					TargetClient->TurnCount = stepTypeData->Data[1];
				}
				TargetClient->StartWaitShowStep(stepType);

				break;
			}

			case MsgContent::CLIENT_INPUT_RPS:
			{
				TargetClient->ShowPiecesThinkMark();
				TargetClient->MainPlayer->StartIterateSelfPiecesInputHand();
				
				break;
			}

			case MsgContent::CLIENT_SET_PIECE_HAND:
			{
				Data_Int4* data = (Data_Int4*)msg.Data;
				int pieceID = data->Data[0];
				HandType handType = (HandType)data->Data[1];
				Piece* piece = TargetClient->Pieces[pieceID];
				{
					//save handtype data in client for check actpoint show
					piece->FinishSetHand(handType);
				}

				break;
			}

			case MsgContent::CLIENT_SHOW_PIECE_HAND:
			{
				//start wait once
				TargetClient->StartWaitPiecesFinish();

				//test
				Data_Int* pieceIDData = (Data_Int*)msg.Data;
				int pieceID = pieceIDData->Data;
				TargetClient->StartPieceShowHand(pieceID);

				break;
			}

			case MsgContent::CLIENT_SHOW_CHECK_ACTPOINT:
			{
				//DebugInfo::Print("[get] show check piece actpoint");

				//test
				Data_Int* pieceIDData = (Data_Int*)msg.Data;
				int pieceID = pieceIDData->Data;
				TargetClient->StartPieceShowCheckActpoint(pieceID);

				break;
			}

			case MsgContent::CLIENT_INPUT_PIECE_ACT:
			{
				//DebugInfo::Print("[get] piece input act");

				//test
				Data_Int* pieceIDData = (Data_Int*)msg.Data;
				int pieceID = pieceIDData->Data;
				TargetClient->StartPieceInputAct(pieceID);

				break;
			}

			case MsgContent::CLIENT_SHOW_PIECE_FINISH:
			{
				//DebugInfo::Print("[get] piece finish act");

				//test
				Data_Int* pieceIDData = (Data_Int*)msg.Data;
				int pieceID = pieceIDData->Data;
				TargetClient->StartPieceShowFinishAct(pieceID);

				break;
			}

			case MsgContent::CLIENT_CLEAR_PIECE_HAND_AND_ACTPOINT:
			{
				//start wait once
				TargetClient->StartWaitPiecesFinish();

				TargetClient->StartPiecesClearHandAndActpoint();
				break;
			}
#pragma endregion

			}
		}
		RecvMsgQueue.pop();
	}

	//check send
	while (!SendMsgQueue.empty())
	{
		SendMsg* sendMsg = SendMsgQueue.front();
		{
			if (IsTCP)
				//tcp
			{
				int nRtn = 0;
				if (TargetServer)
				{
					int tcpSockID = sendMsg->TCPSockID;
					nRtn = send(
						TCPSockets[tcpSockID].Socket,
						sendMsg->MsgBuffer,
						LEN_MSG,
						0
					);

					if (nRtn == SOCKET_ERROR) {
						MessageBox(GetWindow(), "送信失敗です", WINDOW_NAME, MB_OK);
						closesocket(TCPSockets[tcpSockID].Socket);
						DebugInfo::Print("tcp send failed");
					}
					else
					{
						if (sendMsg->PrintRowID != -1)
						{
							DebugInfo::PrintRow(sendMsg->PrintRowID,
								"[send] " + sendMsg->Comment);
						}
					}
				}
				if (TargetClient)
				{
					nRtn = send(
						Socket,
						sendMsg->MsgBuffer,
						LEN_MSG,
						0
					);


					if (nRtn == SOCKET_ERROR) {
						MessageBox(GetWindow(), "送信失敗です", WINDOW_NAME, MB_OK);
						closesocket(Socket);
						DebugInfo::Print("tcp send failed");
					}
					else
					{
						if (sendMsg->PrintRowID != -1)
						{
							DebugInfo::PrintRow(sendMsg->PrintRowID,
								"[send] " + sendMsg->Comment);
						}
					}
				}
			}
			else
				//udp
			{
				int nRtn = sendto(
					Socket,
					sendMsg->MsgBuffer,
					LEN_MSG,
					0,
					(SOCKADDR*)&sendMsg->Destination,
					sizeof(SOCKADDR)
				);
				if (nRtn == SOCKET_ERROR) {
					MessageBox(GetWindow(), "送信失敗です",
						WINDOW_NAME, MB_OK);

					DebugInfo::Print("udp send failed");
					return;
				}
				else
				{
					DebugInfo::Print("udp send success");
				}
			}
		}
		SendMsgQueue.pop();
	}
}

void NetworkManager::TCPMsgRecv(int tcpSockID, LPARAM lp)
{
	int ievent = WSAGETSELECTEVENT(lp);
	switch (ievent)
	{
	case FD_READ:
	{
		if (IsTCP)
			//tcp
		{
			int fromLen = sizeof(SOCKADDR);
			char msgBuf[LEN_MSG];

			//recv msg
			int nRtn = recv(
				TCPSockets[tcpSockID].Socket,
				msgBuf,
				LEN_MSG,
				0);
			{
				//check error
				if (nRtn == SOCKET_ERROR)
				{
					MessageBox(GetWindow(), "受信エラーです", WINDOW_NAME, MB_OK);
					DebugInfo::Print("tcp recv failed");
				}
				else
				{
					//DebugInfo::Print("tcp recv success");

					SOCKADDR_IN from;//useless in tcp
					RecvMsg* recvMsg = new RecvMsg(tcpSockID, msgBuf);

					RecvMsgQueue.push(recvMsg);
				}
			}
		}
		break;
	}

	}
	return;
}
void NetworkManager::SentToAllClients(const char* msgBuf,const std::string& comment)
{
	if (TargetServer)
	{
		for (auto mem : TargetServer->ClientMembers)
		{
			//make sendmsg
			SendMsg* send = nullptr;
			if (IsTCP)
			{
				send = new SendMsg(
					mem.second->TCPSocketID,
					msgBuf);

				send->PrintRowID = DebugInfo::Print("send");
				send->Comment =
					"to client" +
					std::string("[" + std::to_string(mem.second->TCPSocketID) + "] ") +
					comment;
			}
			else
			{
				send = new SendMsg(
					mem.second->Address,
					msgBuf);
			}

			//add to sendmsgqueue
			SendMsgQueue.push(send);
		}
	}
}
#pragma endregion
