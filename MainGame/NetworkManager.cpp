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
	char Name[LEN_CLIENT_NAME];
	char IP[LEN_CLIENT_IP];
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

/*********************************************************
* @brief	SendMsgを便利に作る
********************************************************/
void MakeTCPSendMsg_Char(bool broadcast, int tcpSockID=-1, std::string data="",std::string info="send")
{

}
void MakeTCPSendMsg_Int(bool broadcast, int tcpSockID = -1, std::string data = "", std::string info = "send")
{

}
void MakeTCPSendMsg_Int4(bool broadcast, int tcpSockID = -1, std::string data = "", std::string info = "send")
{

}
void MakeTCPSendMsg_Client(bool broadcast, int tcpSockID = -1, std::string data = "", std::string info = "send")
{

}
#pragma endregion

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

void NetworkManager::CloseTCPSock(int id)
{
	TCPSockets[id].IsUsed = false;
	closesocket(TCPSockets[id].Socket);
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
		TargetClient->m_ServerPort = port;

		//set HostEnt
		unsigned int IP = inet_addr(serverIP.c_str());
		//HostEnt = gethostbyaddr(serverIP.c_str(), 4, AF_INET);
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
				if (TargetClient->m_JoinSuccess)
				{
					DebugInfo::DrawNetMsg();
				}
			}

			if (TargetServer)
			{
				if (TargetServer->m_OpenGameRoom)
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
				TargetClient->m_ServerAddr,
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
		//data for serilize
		Data_Int data;
		data.Data = TargetClient->m_ID;
		MsgContent msg = { MsgContent::SERVER_SET_CLIENT_DISCONNECT,
			LEN_DATA_INT, &data };

		//make sendmsg
		SendMsg* send = nullptr;
		if (IsTCP)
		{
			if (!TargetClient->m_IsConnected)
			{
				return;
			}

			send = new SendMsg(EncodeMsgContent(msg));
			send->PrintRowID = DebugInfo::Print("send");
			send->Comment = "request disconnect";
		}
		else
		{
			send = new SendMsg(
				TargetClient->m_ServerAddr,
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
		memcpy(data.Name, TargetClient->m_Name, LEN_CLIENT_NAME);
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
				TargetClient->m_ServerAddr,
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

			//connect(Socket, (SOCKADDR*)&TargetClient->ServerAddr,LEN_ADDRIN);
		}
	}
}
void NetworkManager::Server_AgreeJoin(ClientMember* member)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int4 data;
		data.Data[0] = member->TCPSocketID;
		data.Data[1] = TargetServer->m_TargetClientNum;
		MsgContent msg = { MsgContent::CLIENT_JOIN_ROOM,
			LEN_DATA_INT4, &data };

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
void NetworkManager::Server_DisagreeJoin(ClientMember* member)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int data;
		data.Data = member->TCPSocketID;
		MsgContent msg = { MsgContent::CLIENT_JOIN_ROOM_FAIL,
			LEN_DATA_INT, &data };

		//make sendmsg
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
				"can't join";
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
void NetworkManager::Server_SendJoinedClientInfosToClient(ClientMember* member)
{
	if (TargetServer)
	{
		for (auto idMem : TargetServer->m_ClientMembers)
		{
			//data for serilize
			Data_Client data;
			ClientMember* memInfo = idMem.second;
			memcpy(data.Name, memInfo->Name.c_str(), LEN_CLIENT_NAME);
			data.ID = memInfo->TCPSocketID;
			data.Ready = memInfo->Ready;
			MsgContent msg = { MsgContent::CLIENT_UPDATE_JOINEDNUM,
				LEN_DATA_CLIENT, &data };

			//make sendmsg
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
					"client["+ std::to_string(member->TCPSocketID) + "]info";
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
void NetworkManager::Server_CommandUpdateJoinedNum(ClientMember* member)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Client data;
		//data0:joinedNum,data1:targetNum
		memcpy(data.Name, member->Name.c_str(), LEN_CLIENT_NAME);
		memcpy(data.IP, inet_ntoa(member->Address.sin_addr), 64);
		data.ID = member->TCPSocketID;
		data.Ready = member->Ready;
		MsgContent msg = { MsgContent::CLIENT_UPDATE_JOINEDNUM,
			LEN_DATA_CLIENT,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf,"command update joined num");
	}
}
void NetworkManager::Client_RequestSetReady(bool ready)
{
	if (TargetClient)
	{
		//data for serilize
		Data_Int4 data;
		data.Data[0] = TargetClient->m_ID;
		data.Data[1] = ready?1:0;//0:not ready, 1:ready
		MsgContent msg = { MsgContent::SERVER_SET_CLIENT_READY,
			LEN_DATA_INT4,&data };
		char* msgBuf = EncodeMsgContent(msg);

		//make sendmsg
		SendMsg* send = nullptr;
		if (IsTCP)
		{
			send = new SendMsg(EncodeMsgContent(msg));
			send->PrintRowID = DebugInfo::Print("send");
			send->Comment = "request set ready";
		}
		else
		{
			send = new SendMsg(
				TargetClient->m_ServerAddr,
				EncodeMsgContent(msg));
		}

		//add to sendmsgqueue
		SendMsgQueue.push(send);
	}
}

void NetworkManager::Judgement_CommandCreatePlayer()
{
	if (TargetServer)
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
			MsgContent msg = { MsgContent::CLIENT_CREATE_PLAYER,
				LEN_DATA_PLAYERDESC,&data };

			//make sendmsg
			ClientMember* member = TargetServer->m_ClientMembers[player->m_ID];
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
		Data_Int data{ GameManager::m_Pieces.size() };
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
			piece->m_CharaType,
			piece->m_Camp,
			piece->m_ID,
			piece->m_OwnerPlayer->m_ID
		};
		MsgContent msg = { MsgContent::CLIENT_CREATE_PIECE,
			LEN_DATA_PIECEDESC,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf, "command create piece " + std::to_string(piece->m_ID));
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
		Data_Int4 data{ piece->m_ID, piece->m_FootOnSquare->m_ID };
		MsgContent msg = { MsgContent::CLIENT_SHOW_PIECE_ENTRY,
			LEN_DATA_INT4,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf,
			"command show piece " +
			std::to_string(piece->m_ID) +
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
			data.Data[1] = Judgement::Instance()->m_TurnCount;
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
		Data_Int data{ piece->m_ID };
		MsgContent msg = { MsgContent::CLIENT_SHOW_PIECE_HAND,
			LEN_DATA_INT,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf,
			"command show piece " +
			std::to_string(piece->m_ID) +
			" hand[" +
			Hand::HandName(*piece->m_Hands.begin()) +
			"]");
	}
}
void NetworkManager::Judgement_CommandPieceShowCheckActpoint(Piece* piece)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int4 data{ piece->m_ID,piece->m_ActPoint };
		MsgContent msg = { MsgContent::CLIENT_SHOW_CHECK_ACTPOINT,
			LEN_DATA_INT4,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf,
			"command piece " +
			std::to_string(piece->m_ID) +
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
				TargetClient->m_ServerAddr,
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
				TargetClient->m_ServerAddr,
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
				TargetClient->m_ServerAddr,
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
		Data_Int data{ piece->m_ID };
		MsgContent msg = { MsgContent::CLIENT_INPUT_PIECE_ACT,
			LEN_DATA_INT,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf,
			"command piece["+
			std::to_string(piece->m_ID) + 
			"] input act");
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
				TargetClient->m_ServerAddr,
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
			"command piece[" +
			std::to_string(pieceID) +
			"] finish act");
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
				TargetClient->m_ServerAddr,
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
			"command piece[" +
			std::to_string(pieceID) +
			"] move to square[" +
			std::to_string(squareID) +
			"]");
	}
}
void NetworkManager::Judgement_CommandPieceContinueMove(ClientMember* member, int pieceID)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int data;
		data.Data = pieceID;
		MsgContent msg = { MsgContent::CLIENT_CONTINUE_INPUT_PIECE_MOVE,
			LEN_DATA_INT, &data };

		//make sendmsg
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
				"continue move";
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
void NetworkManager::Judgement_CommandPieceCaught(int movePieceID, int caughtPieceID, int prisonRoomSquareID)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int4 data;
		data.Data[0] = movePieceID;
		data.Data[1] = caughtPieceID;
		data.Data[2] = prisonRoomSquareID;
		MsgContent msg = { MsgContent::CLIENT_SHOW_PIECE_CAUGHT,
			LEN_DATA_INT4, &data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf,
			"command piece[" +
			std::to_string(movePieceID) +
			"] catch piece[" +
			std::to_string(caughtPieceID) +
			"]");
	}
}
void NetworkManager::Judgement_CommandPieceEscape(int escapePieceID, int goalSquareID)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int4 data;
		data.Data[0] = escapePieceID;
		data.Data[1] = goalSquareID;
		MsgContent msg = { MsgContent::CLIENT_SHOW_PIECE_ESCAPE,
			LEN_DATA_INT4, &data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf,
			"command piece[" +
			std::to_string(escapePieceID) +
			"] escape");
	}
}


void NetworkManager::Judgement_CommandPiecesClearHandAndActpoint()
{
	if (TargetServer)
	{
		//data for serilize
		MsgContent msg = { MsgContent::CLIENT_CLEAR_PIECE_HAND_AND_ACTPOINT,
			LEN_ZERO,nullptr };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf, "command clear hand and actpoint ");
	}
}
void NetworkManager::Judgement_CommandPieceClearActpoint(Piece* piece)
{

}
void NetworkManager::Judgement_CommandPieceClearHand(Piece* piece)
{

}
void NetworkManager::Client_RequestUpdatePieceActpoint(Piece* piece)
{

}
void NetworkManager::Client_RequestUpdatePieceHand(Piece* piece)
{

}
void NetworkManager::Judgement_CommandShowGameOver(int result)
{
	if (TargetServer)
	{
		//data for serilize
		Data_Int data{ result };
		MsgContent msg = { MsgContent::CLIENT_SHOW_GAME_OVER,
			LEN_DATA_INT,&data };
		char* msgBuf = EncodeMsgContent(msg);
		SentToAllClients(msgBuf, "command show game over ");
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

			DebugInfo::Print("try make accept client socket...");

			SOCKADDR_IN from;
			int fromLen = sizeof(SOCKADDR);
			int id = GetEnableTCPID();

			//when id out of sockets' max, don't accept
			if (id == -1)
			{
				DebugInfo::Print("sockets is full, can't accept");
				break;
			}

			//accept, check if need to set up tcp socket
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
			if (TargetClient)
			{
				TargetClient->CheckConnect();
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
			{
				Data_Client* clientData = (Data_Client*)msg.Data;
				std::string name = clientData->Name;
				int id = recv->TCPSockID;//tips: tcpClientID=tcpSockID
				TargetServer->StartCheckClientJoin(id, name);

				break;
			}

			case MsgContent::SERVER_SET_CLIENT_DISCONNECT:
			{
				Data_Int* clientIDData = (Data_Int*)msg.Data;
				int id = clientIDData->Data;
				TargetServer->SetClientDisconnect(id);
				
				break;
			}

			case MsgContent::SERVER_SET_CLIENT_READY:
			{
				Data_Int4* clientData = (Data_Int4*)msg.Data;
				int id = clientData->Data[0];
				bool ready = clientData->Data[1] == 1 ? true : false;//0:not ready, 1:ready
				TargetServer->SetClientReady(id, ready);

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
				GameManager::m_Pieces[pieceID]->AddHand(handType);

				//broadcast to client
				Judgement_CommandSetHand(pieceID, handType);

				break;
			}

			case MsgContent::JUDGEMENT_CHECK_PIECE_MOVE:
			{
				Data_Int4* data = (Data_Int4*)msg.Data;
				int pieceID = data->Data[0];
				int squareID = data->Data[1];

				Judgement::Instance()->CheckPieceMove(pieceID, squareID);

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
				Data_Int4* clientIDData = (Data_Int4*)msg.Data;
				int id = clientIDData->Data[0];
				int targetClientNum = clientIDData->Data[1];
				TargetClient->JoinRoom(id, targetClientNum);

				break;
			}

			case MsgContent::CLIENT_UPDATE_JOINEDNUM:
			{
				Data_Client* memberData = (Data_Client*)msg.Data;
				TargetClient->UpdateClientInfo(
					memberData->ID,
					memberData->Name,
					memberData->Ready);

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
				if (GameManager::m_Pieces.size() ==
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
					TargetClient->m_TurnCount = stepTypeData->Data[1];
				}
				TargetClient->StartWaitShowStep(stepType);

				break;
			}

			case MsgContent::CLIENT_INPUT_RPS:
			{
				TargetClient->ShowPiecesThinkMark();
				TargetClient->m_MainPlayer->StartIterateSelfPiecesInputHand();
				
				break;
			}

			case MsgContent::CLIENT_SET_PIECE_HAND:
			{
				Data_Int4* data = (Data_Int4*)msg.Data;
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
				Data_Int4* data = (Data_Int4*)msg.Data;
				int pieceID = data->Data[0];
				int actPoint = data->Data[1];
				TargetClient->StartPieceShowCheckActpoint(pieceID, actPoint);

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
				TargetClient->StartWaitPiecesFinish();
				TargetClient->StartPiecesClearHandAndActpoint();

				break;
			}


			case MsgContent::CLIENT_SHOW_PIECE_MOVE:
			{
				Data_Int4* moveData = (Data_Int4*)msg.Data;
				int pieceID = moveData->Data[0];
				int squareID = moveData->Data[1];
				TargetClient->StartPieceShowMove(pieceID, squareID);

				break;
			}

			case MsgContent::CLIENT_CONTINUE_INPUT_PIECE_MOVE:
			{
				Data_Int* moveData = (Data_Int*)msg.Data;
				int pieceID = moveData->Data;

				Piece* piece = GameManager::m_Pieces[pieceID];
				piece->StartInputMove();

				break;
			}

			case MsgContent::CLIENT_SHOW_PIECE_CAUGHT:
			{
				Data_Int4* caughtData = (Data_Int4*)msg.Data;
				int movePieceID = caughtData->Data[0];
				int caughtPieceID = caughtData->Data[1];
				int prisonRoomSquareID = caughtData->Data[2];
				TargetClient->StartPieceShowCaught(movePieceID, caughtPieceID, prisonRoomSquareID);

				break;
			}

			case MsgContent::CLIENT_SHOW_PIECE_ESCAPE:
			{
				Data_Int4* escapeData = (Data_Int4*)msg.Data;
				int escapePieceID = escapeData->Data[0];
				int escapeSquareID = escapeData->Data[1];
				TargetClient->StartPieceShowEscape(escapePieceID, escapeSquareID);

				break;
			}

			case MsgContent::CLIENT_SHOW_GAME_OVER:
			{
				Data_Int* gameOverData = (Data_Int*)msg.Data;
				int result = gameOverData->Data;
				TargetClient->StartShowGameOver(result);

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

					if (TCPSockets[tcpSockID].IsUsed == false)
					{
						//don't send
						SendMsgQueue.pop();
						continue;
					}

					nRtn = send(
						TCPSockets[tcpSockID].Socket,
						sendMsg->MsgBuffer,
						LEN_MSG,
						0
					);

					if (nRtn == SOCKET_ERROR) 
					{
						MessageBox(GetWindow(), "送信失敗です", WINDOW_NAME, MB_OK);
						closesocket(TCPSockets[tcpSockID].Socket);
						TCPSockets[tcpSockID].IsUsed = false;
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


					if (nRtn == SOCKET_ERROR) 
					{
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
		for (auto mem : TargetServer->m_ClientMembers)
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
