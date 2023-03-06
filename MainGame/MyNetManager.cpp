#include "main.h"
#include "SerializedTool.h"
#include "MyNetManager.h"
#include <iostream>

#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "Client.h"
#include "Server.h"

using namespace std;

#pragma region ========== network update net proc ==========
NetworkUpdateProc::NetworkUpdateProc(NetworkManager_AS* owner)
{
	m_ASManager = owner;
	m_ASManager->m_NetworkUpdateProcObj = this;
}

NetworkUpdateProc::~NetworkUpdateProc()
{
	m_ASManager->m_NetworkUpdateProcObj = nullptr;
}

Server_AS::Server_AS(NetworkManager_AS* owner) :
	Server(owner),
	NetworkUpdateProc(owner)
{
#if 0
	//todo
	//customize
	{
		//set socket async select in event  
#pragma region ========== add event ==========
		AddEvent(EventType::SucceedLaunchServer,
			[this](int arg)
			{
				cout << "server_as launch ok" << endl;
			});
		AddEvent(EventType::SucceedBindTCPListen,
			[this](int arg)
			{
				cout << "TCPListenSocket bind port[" << m_TCPPort << "] ok" << endl;
			});
		AddEvent(EventType::SucceedBindUDP,
			[this](int arg)
			{
				cout << "UDPSocket bind port[" << m_TCPPort << "] ok" << endl;

				//UDPSocket select
				{
					int ret = WSAAsyncSelect(
						m_UDPSocket,
						GetWindow(),
						NET_MSG + OFFSET_NET_MSG_UDP,
						FD_READ | FD_WRITE);
				}
			});
		AddEvent(EventType::SucceedStartListen,
			[this](int arg)
			{
				cout << "TCPListenSocket start listen ok" << endl;

				//TCPListenSocket select
				{
					int ret = WSAAsyncSelect(
						m_TCPListenSocket,
						GetWindow(),
						NET_MSG + OFFSET_NET_MSG_LISTEN,
						FD_ACCEPT);
				}
			});
		AddEvent(EventType::SucceedAccept,
			[this](int clientID)
			{
				cout << "accept client by socket[" << clientID << "] ok" << endl;

				//TCPSocket select
				{
					int ret = WSAAsyncSelect(
						m_TCPSockets[clientID],
						GetWindow(),
						NET_MSG + OFFSET_NET_MSG_TCP + clientID,
						FD_READ | FD_WRITE);
				}
			});
#pragma endregion
		
		//deal with msg
#pragma region ========== set recv and send process ==========
		m_Manager->SetRecvProcess(
			[this](TransportMsg* tmsg)
			{
				MsgContent content;
				DecodeMsgContent(tmsg->m_MsgBuffer, content);

				//Data_Char256* data = (Data_Char256*)content.Data;
				//cout << data->Data << endl;
				//AddSendMsgsToAll(tmsg->m_MsgBuffer, SEND_BY_TCP);
			});
		m_Manager->SetSendProcess(
			[this](TransportMsg* tmsg)
			{
				if (!tmsg->m_UDPAddr)
					//tcp
				{
					Send(tmsg->m_Socket, tmsg->m_MsgBuffer);
				}
				else
					//udp
				{
					SendTo(tmsg->m_Socket, tmsg->m_MsgBuffer, tmsg->m_UDPAddr);
				}
			});
#pragma endregion
	}
#endif
}

void Server_AS::UpdateNetProc(UINT uWinMsg, LPARAM lParam)
{
	if (!m_StartUp)
	{
		return;
	}

	//check uWinMsg and NET_MSG+(x)
	int ievent = WSAGETSELECTEVENT(lParam);
	int msgOffset = uWinMsg - NET_MSG;

	//check is net msg
	if (msgOffset >= OFFSET_NET_MSG_MAX)
	{
		return;
	}

	if (msgOffset == OFFSET_NET_MSG_UDP)
		//udp sock
	{
		if (ievent == FD_READ)
		{
			SOCKADDR_IN from;
			SOCKET* socket = &m_UDPSocket;
			RecvFrom(socket, &from);
			return;
		}
	}
	else if (msgOffset == OFFSET_NET_MSG_LISTEN)
		//tcp listen sock
	{
		if (ievent == FD_ACCEPT)
		{
			Accept();
			return;
		}
	}
	else
		//tcp sock
	{
		if (ievent == FD_READ)
		{
			int tcpSockID = msgOffset - OFFSET_NET_MSG_TCP;
			SOCKET* socket = &m_TCPSockets[tcpSockID];
			Recv(socket);
			return;
		}
	}
}

Client_AS::Client_AS(NetworkManager_AS* owner) :
	Client(owner) ,
	NetworkUpdateProc(owner)
{
#if 0
	//todo
	//customize
	{
		//set socket async select in event  
#pragma region ========== add event ==========
		AddEvent(EventType::SucceedLaunchClient,
			[this](int arg)
			{
				cout << "client_as launch ok" << endl;

				//UDPSocket select
				{
					int ret = WSAAsyncSelect(
						m_UDPSocket,
						GetWindow(),
						NET_MSG + OFFSET_NET_MSG_UDP,
						FD_READ | FD_WRITE);
				}
			});
		AddEvent(EventType::BeforeTryConnect,
			[this](int arg)
			{
				//TCPSocket select
				if(1)
				{
					cout << "TCPSocket async select" << endl;
					int ret = WSAAsyncSelect(
						m_TCPSocket,
						GetWindow(),
						NET_MSG + OFFSET_NET_MSG_TCP,
						FD_READ | FD_WRITE | FD_CONNECT);
				}
			});
		AddEvent(EventType::FailTryConnect,
			[this](int arg)
			{
				//tips: failed because it is async select
				//cout << "TCPSocket try connect fail" << endl;
			});
		AddEvent(EventType::SucceedTryConnect,
			[this](int arg)
			{
				cout << "TCPSocket try connect ok" << endl;
			});
		AddEvent(EventType::FailConnect,
			[this](int arg)
			{
				cout << "TCPSocket connect fail" << endl;
			});
		AddEvent(EventType::SucceedConnect,
			[this](int arg)
			{
				cout << "TCPSocket connect server[" << m_ServerIP << "," << m_ServerTCPPort << "] ok" << endl;
			});
#pragma endregion

		//deal with msg
#pragma region ========== set recv and send process ==========
		m_Manager->SetRecvProcess(
			[this](TransportMsg* msg)
			{
				MsgContent content;
				DecodeMsgContent(msg->m_MsgBuffer, content);
				
				//Data_Char256* data = (Data_Char256*)content.Data;
				//cout << data->Data << endl;
			});
		m_Manager->SetSendProcess(
			[this](TransportMsg* tmsg)
			{
				if (!tmsg->m_UDPAddr)
					//tcp
				{
					Send(tmsg->m_Socket, tmsg->m_MsgBuffer);
				}
				else
					//udp
				{
					SendTo(tmsg->m_Socket, tmsg->m_MsgBuffer, tmsg->m_UDPAddr);
				}
			});
#pragma endregion
	}
#endif
};

void Client_AS::UpdateNetProc(UINT uWinMsg, LPARAM lParam)
{
	if (!m_StartUp)
	{
		return;
	}

	//check uWinMsg and NET_MSG+(x)
	int ievent = WSAGETSELECTEVENT(lParam);
	int msgOffset = uWinMsg - NET_MSG;

	//check is net msg
	if (msgOffset >= OFFSET_NET_MSG_MAX)
	{
		return;
	}

	if (msgOffset == OFFSET_NET_MSG_UDP)
		//udp sock
	{
		if (ievent == FD_READ)
		{
			SOCKADDR_IN from;
			SOCKET* socket = &m_UDPSocket;
			RecvFrom(socket, &from);
			return;
		}
	}
	else if (msgOffset == OFFSET_NET_MSG_TCP)
		//tcp sock
	{
		if (ievent == FD_READ)
		{
			SOCKET* socket = &m_TCPSocket;
			Recv(socket);
			return;
		}
		if (ievent == FD_CONNECT)
		{
			CheckConnect();
			return;
		}
	}
}
#pragma endregion

#pragma region ========== network manager (async select) ==========
void NetworkManager_AS::UpdateNetProc(UINT uWinMsg, LPARAM lParam)
{
	if (m_NetworkObj)
	{
		m_NetworkUpdateProcObj->UpdateNetProc(uWinMsg, lParam);
	}
}
#pragma endregion

#pragma region ========== my server ==========
MyServer::MyServer(MyNetManager* owner):
	Server_AS(owner),
	m_MyNetManager(owner)
{
	//set socket async select in event  
#pragma region ========== add event ==========
	AddEvent(EventType::SucceedLaunchServer,
		[this](int arg)
		{
			DebugInfo::Print("server_as launch ok");
		});
	AddEvent(EventType::SucceedBindTCPListen,
		[this](int arg)
		{
			DebugInfo::Print("TCPListenSocket bind port[" + std::to_string(m_TCPPort) + "] ok");
		});
	AddEvent(EventType::SucceedBindUDP,
		[this](int arg)
		{
			DebugInfo::Print("UDPSocket bind port[" + std::to_string(m_UDPPort) + "] ok");

			//UDPSocket select
			{
				int ret = WSAAsyncSelect(
					m_UDPSocket,
					GetWindow(),
					NET_MSG + OFFSET_NET_MSG_UDP,
					FD_READ | FD_WRITE);
			}
		});
	AddEvent(EventType::SucceedStartListen,
		[this](int arg)
		{
			DebugInfo::Print("TCPListenSocket start listen ok");

			//TCPListenSocket select
			{
				int ret = WSAAsyncSelect(
					m_TCPListenSocket,
					GetWindow(),
					NET_MSG + OFFSET_NET_MSG_LISTEN,
					FD_ACCEPT);
			}
		});
	AddEvent(EventType::SucceedAccept,
		[this](int clientID)
		{
			DebugInfo::Print("accept client by socket[" + std::to_string(clientID) + "] ok");

			//TCPSocket select
			{
				int ret = WSAAsyncSelect(
					m_TCPSockets[clientID],
					GetWindow(),
					NET_MSG + OFFSET_NET_MSG_TCP + clientID,
					FD_READ | FD_WRITE);
			}

			//command client to set this clientID
			GetNetSendFunc().Server_CommandClientSetID(clientID);
		});
#pragma endregion

	//deal with msg
#pragma region ========== set recv and send process ==========
	m_Manager->SetRecvProcess(
		[this](TransportMsg* tmsg)
		{
			//auto sendOp = (SendOperate*)tmsg;
			MsgContent content;
			DecodeMsgContent(tmsg->m_MsgBuffer, content);
			GetNetSendFunc().Server_CheckRecv(content);
		});
	m_Manager->SetSendProcess(
		[this](TransportMsg* tmsg)
		{
			//use tcp
			auto sendOp = (SendOperate*)tmsg;
			int ret = Send(sendOp->m_Socket, sendOp->m_MsgBuffer);
			if (ret == SOCKET_ERROR)
			{
				DebugInfo::PrintRow(sendOp->m_PrintRowID,
					"[fail] to " +
					sendOp->m_DestInfo +
					sendOp->m_Comment);
			}
			else
			{
				DebugInfo::PrintRow(sendOp->m_PrintRowID,
					"[success] to " +
					sendOp->m_DestInfo +
					sendOp->m_Comment);
			}
		});
#pragma endregion
}
void MyServer::SendToOne(int clientID, const MsgContent& content, std::string comment)
{
	//use tcp
	m_MyNetManager->AddSendMsg(
		new SendOperate(
			&m_TCPSockets[clientID],
			EncodeMsgContent(content),
			nullptr,
			DebugInfo::Print("send"),
			"client[" + std::to_string(clientID) + "] ", 
			comment));
}
void MyServer::SendToAll(const MsgContent& content, std::string comment)
{
	//use tcp
	for (auto idSock : m_TCPSockets)
	{
		SendToOne(idSock.first, content, comment);
	}
}
void MyServer::SendToOther(int exceptClientID, const MsgContent& content, std::string comment)
{
	//use tcp
	for (auto idSock : m_TCPSockets)
	{
		if (idSock.first != exceptClientID)
		{
			SendToOne(idSock.first, content, comment);
		}
	}
}
#pragma endregion

#pragma region ========== my client ==========
MyClient::MyClient(MyNetManager* owner):
	Client_AS(owner),
	m_MyNetManager(owner)
{
	//set socket async select in event  
#pragma region ========== add event ==========
	AddEvent(EventType::SucceedLaunchClient,
		[this](int arg)
		{
			DebugInfo::Print("client_as launch ok");

			//UDPSocket select
			{
				int ret = WSAAsyncSelect(
					m_UDPSocket,
					GetWindow(),
					NET_MSG + OFFSET_NET_MSG_UDP,
					FD_READ | FD_WRITE);
			}
		});
	AddEvent(EventType::BeforeTryConnect,
		[this](int arg)
		{
			//TCPSocket select
			if (1)
			{
				DebugInfo::Print("TCPSocket async select");
				int ret = WSAAsyncSelect(
					m_TCPSocket,
					GetWindow(),
					NET_MSG + OFFSET_NET_MSG_TCP,
					FD_READ | FD_WRITE | FD_CONNECT);
			}
		});
	AddEvent(EventType::SucceedTryConnect,
		[this](int arg)
		{
			DebugInfo::Print("TCPSocket try connect ok");
		});
	AddEvent(EventType::FailConnect,
		[this](int arg)
		{
			DebugInfo::Print("TCPSocket connect fail");
		});
	AddEvent(EventType::SucceedConnect,
		[this](int arg)
		{
			DebugInfo::Print(
				"TCPSocket connect server[" + 
				m_ServerIP + "," + 
				std::to_string(m_ServerTCPPort) + 
				"] ok");

			//check connect and continue BH_TryConnect
			AppClient::Instance()->CheckConnect();
		});
#pragma endregion

	//deal with msg
#pragma region ========== set recv and send process ==========
	m_Manager->SetRecvProcess(
		[this](TransportMsg* tmsg)
		{
			auto sendOp = (SendOperate*)tmsg;
			MsgContent content;
			DecodeMsgContent(tmsg->m_MsgBuffer, content);
			GetNetSendFunc().Client_CheckRecv(content);
		});
	m_Manager->SetSendProcess(
		[this](TransportMsg* tmsg)
		{
			//use tcp
			auto sendOp = (SendOperate*)tmsg;
			int ret = Send(sendOp->m_Socket, sendOp->m_MsgBuffer);
			if (ret == SOCKET_ERROR)
			{
				DebugInfo::PrintRow(sendOp->m_PrintRowID,
					"[fail] to " +
					sendOp->m_DestInfo +
					sendOp->m_Comment);
			}
			else
			{
				DebugInfo::PrintRow(sendOp->m_PrintRowID,
					"[success] to " +
					sendOp->m_DestInfo +
					sendOp->m_Comment);
			}
		});
#pragma endregion
}
void MyClient::SendToServer(const MsgContent& content, std::string comment)
{
	//use tcp
	m_MyNetManager->AddSendMsg(
		new SendOperate(
			&m_TCPSocket,
			EncodeMsgContent(content),
			nullptr,
			DebugInfo::Print("send"),
			"server ",
			comment));
}
#pragma endregion

#pragma region ========== my network manager ==========
void MyNetManager::AddSendMsg(SendOperate* sendOp)
{
	m_SendMsgQueue.push(sendOp);
}
void MyNetManager::SetChatPanel(bool show)
{
	if (show)
	{
		//test
		DebugInfo::TestBlocks.emplace(TESTBLOCKID_NETWORK_CHAT, [this]()
			{
				if (m_TargetAppClient)
				{
					if (m_TargetAppClient->m_JoinSuccess)
					{
						DebugInfo::DrawNetMsg();
					}
				}

				if (m_TargetAppServer)
				{
					if (m_TargetAppServer->m_OpenGameRoom)
					{
						DebugInfo::DrawNetMsg();
					}
				}
			});
	}
	else
	{
		DebugInfo::CloseBlock(TESTBLOCKID_NETWORK_CHAT);
	}
}
#pragma endregion

