#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <iostream>

#include <windows.h>
#include <windowsx.h>
#include <winsock2.h>


#pragma comment (lib, "Ws2_32.lib")
#include "NetworkManager.h"
#include "NetworkObject.h"

#pragma region ========== network object ==========
NetworkObject::NetworkObject(NetworkManager* owner)
{
	m_Manager = owner;
	m_Manager->m_NetworkObj = this;
}
NetworkObject::~NetworkObject()
{
	m_Manager->m_NetworkObj = nullptr;
}
void NetworkObject::Launch()
{
	//try wsa start up
	int ret = WSAStartup(MAKEWORD(2, 2), &m_WsaData);
	assert(ret == 0, "WSA start error");

	//set start flag
	m_StartUp = true;

	//event
	OnEvent(EventType::SucceedLaunch);
}

void NetworkObject::ShutDown()
{
	if (!m_StartUp)
	{
		return;
	}

	//event
	OnEvent(EventType::BeforeShutDown);

	//try wsa clean up
	assert(WSACleanup() == 0, "WSA clean error");
	
	//event
	OnEvent(EventType::SucceedShutDown);
}

void NetworkObject::Recv(SOCKET* socket)
{
	if (!m_StartUp)
	{
		return;
	}

	char msgBuf[LEN_MSG];

	//recv
	int ret = recv(
		*socket,
		msgBuf,
		LEN_MSG,
		0);

	//check error
	if (ret == SOCKET_ERROR)
	{
		//todo 
		//make error default msgBuf
		//m_Manager->AddRecvMsg(nullptr, msgBuf);
	}
	else
	{
		m_Manager->AddRecvMsg(socket, msgBuf);
	}
}

int NetworkObject::Send(SOCKET* socket, char* msgBuf)
{
	if (!m_StartUp)
	{
		return -2;
	}

	//recv
	int ret = send(
		*socket,
		msgBuf,
		LEN_MSG,
		0);

	//check error
	return ret;
}

void NetworkObject::RecvFrom(SOCKET* socket, SOCKADDR_IN* from)
{
	if (!m_StartUp)
	{
		return;
	}

	char msgBuf[LEN_MSG];
	int fromLen = sizeof(SOCKADDR);

	//recvfrom
	int ret = recvfrom(
		*socket,
		msgBuf,
		LEN_MSG,
		0,
		(SOCKADDR*)from,
		&fromLen);

	//check error
	if (ret == SOCKET_ERROR)
	{
		//todo 
		//make error default msgBuf
		//m_Manager->AddRecvMsg(nullptr, msgBuf, from);
	}
	else
	{
		m_Manager->AddRecvMsg(socket, msgBuf, from);
	}
}

int NetworkObject::SendTo(SOCKET* socket, char* msgBuf, SOCKADDR_IN* dest)
{
	if (!m_StartUp)
	{
		return -2;
	}

	int destLen = sizeof(SOCKADDR);

	//recvfrom
	int ret = sendto(
		*socket,
		msgBuf,
		LEN_MSG,
		0,
		(SOCKADDR*)dest,
		destLen);

	//check error
	return ret;
}


void NetworkObject::AddEvent(EventType type, std::function<void(int arg)> eventFunc)
{
	m_Events[(int)type].emplace_back(eventFunc);
}

void NetworkObject::ClearEvent(EventType type)
{
	m_Events[(int)type].clear();
}

void NetworkObject::OnEvent(EventType type, int arg)
{
	for (auto func : m_Events[(int)type])
	{
		func(arg);
	}
}

#pragma endregion


#pragma region ========== server ==========
int g_EnableIDIndex = 0;
void Server::Launch()
{
	NetworkObject::Launch();

	//set HostEnt by this pc
	char hostName[256];
	gethostname(hostName, (int)sizeof(hostName));
	m_HostEnt = gethostbyname(hostName);

	//set IP by host
	IN_ADDR inaddr;
	memcpy(&inaddr, m_HostEnt->h_addr_list[1], 4);
	m_IP = std::string(inet_ntoa(inaddr));

	//create TCPListenSocket
	m_TCPListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	assert(m_TCPListenSocket > 0, "create TCPListenSocket error");

	//create TCPSocket wait for connect
	m_TCPSockets.clear();

	//create UDPSocket
	m_UDPSocket = socket(AF_INET, SOCK_DGRAM, 0);
	assert(m_UDPSocket > 0, "create UDPSocket error");

	//event
	OnEvent(EventType::SucceedLaunchServer);
}

void Server::Bind()
{
	if (!m_StartUp)
	{
		return;
	}

	SOCKADDR_IN socketAddr;
	ZeroMemory(&socketAddr, LEN_ADDRIN);
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_port = htons(m_TCPPort);
	socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	//TCPListenSocket bind
	{
		int ret = bind(m_TCPListenSocket, (LPSOCKADDR)&socketAddr, LEN_ADDRIN);
		if (ret == SOCKET_ERROR)
			//failed
		{
			closesocket(m_TCPListenSocket);
			OnEvent(EventType::FailBindTCP);
			return;
		}
		else
			//success
		{
			OnEvent(EventType::SucceedBindTCPListen);
		}
	}

	//UDPSocket bind
	{
		socketAddr.sin_port = htons(m_UDPPort);
		int ret = bind(m_UDPSocket, (LPSOCKADDR)&socketAddr, LEN_ADDRIN);
		if (ret == SOCKET_ERROR)
			//failed
		{
			closesocket(m_UDPSocket);
			OnEvent(EventType::FailBindUDP);
			return;
		}
		else
			//success
		{
			OnEvent(EventType::SucceedBindUDP);
		}
	}
}

void Server::Listen()
{
	if (!m_StartUp)
	{
		return;
	}

	//TCPListenSocket start listen
	int ret = listen(m_TCPListenSocket, 0);
	if (ret == SOCKET_ERROR)
		//failed
	{
		closesocket(m_TCPListenSocket);
		OnEvent(EventType::FailStartListen);
		return;
	}
	else
		//success
	{
		//todo
		//WSAAsyncSelect
		OnEvent(EventType::SucceedStartListen);
	}
}

void Server::Accept()
{
	if (!m_StartUp)
	{
		return;
	}

	//check can accept
	if (m_TCPSockets.size() >= MAX_CLIENT_NUM)
	{
		OnEvent(EventType::CannotAccept);
		return;
	}

	//accept by TCPListenSocket and set up TCPSocket
	int clientID = GetEnableClientID();
	int fromLen = sizeof(SOCKADDR);
	SOCKET tempSock;
	SOCKADDR_IN tempAddr;
	tempSock = accept(m_TCPListenSocket, (SOCKADDR*)&tempAddr, &fromLen);
	if (tempSock == INVALID_SOCKET)
		//fail
	{
		OnEvent(EventType::FailAccept);
		return;
	}
	else
		//success
	{
		m_TCPSockets.emplace(clientID, tempSock);//copy val
		m_UDPAddrs.emplace(clientID, tempAddr);//copy val
		OnEvent(EventType::SucceedAccept, clientID);
		return;
	}
}

void Server::Reset(char* msgBuf)
{
	//send for disconnect
	for (auto idSock : m_TCPSockets)
	{
		SOCKET* sock = &idSock.second;
		Send(sock, msgBuf);
	}

	//reset value
	m_IP = std::string(DEFAULT_IP);
	m_TCPPort = DEFAULT_PORT;
	m_UDPPort = DEFAULT_PORT + OFFSET_UDP_PORT;
	m_HostEnt = nullptr;

	//close listen sock
	closesocket(m_UDPSocket);
	closesocket(m_TCPListenSocket);

	//reset enable id
	for (int i = 0; i < MAX_CLIENT_NUM; i++)
	{
		if (m_ClientIDs[i])
		{
			//eject clients connect
			m_TCPSockets.erase(i);
			m_UDPAddrs.erase(i);
		}
		FreeClientID(i);
	}
	m_TCPSockets.clear();
	m_UDPAddrs.clear();
}

void Server::AddSendMsgsToAll(const char* msgBuf, bool tcp)
{
	if (tcp)
		//tcp
	{
		for (auto tcpSock : m_TCPSockets)
		{
			m_Manager->AddSendMsg(&tcpSock.second, msgBuf);
		}
	}
	else
		//udp
	{
		for (auto& udpAddr : m_UDPAddrs)
		{
			m_Manager->AddSendMsg(&m_UDPSocket, msgBuf, &udpAddr.second);
		}
	}
}

int Server::GetEnableClientID()
{
	int count = 0;
	int id = g_EnableIDIndex;
	while (count < MAX_CLIENT_NUM)
	{
		if (m_ClientIDs[id] == false)
		{
			m_ClientIDs[id] = true;
			g_EnableIDIndex = id + 1;
			g_EnableIDIndex %= MAX_CLIENT_NUM;
			return id;
		}
		else
		{
			id++;
			id %= MAX_CLIENT_NUM;
			count++;
		}
	}
	return -1;
}

void Server::FreeClientID(int id)
{
	m_ClientIDs[id] = false;
	if (g_EnableIDIndex > id)
	{
		g_EnableIDIndex = id;
	}
}

#pragma endregion


#pragma region ========== client ==========
void Client::Launch()
{
	NetworkObject::Launch();

	//set serverIP
	/*IN_ADDR inaddr;
	memcpy(&inaddr, m_ServerHostEnt->h_addr_list[0], 4);
	m_ServerIP = std::string(inet_ntoa(inaddr));*/

	//create udp socket
	m_UDPSocket = socket(AF_INET, SOCK_DGRAM, 0);
	assert(m_UDPSocket > 0, "create UDPSocket error");

	//event
	OnEvent(EventType::SucceedLaunchClient);
}

void Client::TryConnect()
{
	//set HostEnt by ip
	m_ServerHostEnt =
		gethostbyname(m_ServerIP.c_str());

	// サーバーへ接続するための準備
	//tcp
	ZeroMemory(&m_ServerTCPAddr, LEN_ADDRIN);
	m_ServerTCPAddr.sin_family = AF_INET;
	m_ServerTCPAddr.sin_port = htons(m_ServerTCPPort);
	m_ServerTCPAddr.sin_addr.s_addr = *((unsigned long*)m_ServerHostEnt->h_addr);
	//udp
	ZeroMemory(&m_ServerUDPAddr, LEN_ADDRIN);
	m_ServerUDPAddr.sin_family = AF_INET;
	m_ServerUDPAddr.sin_port = htons(m_ServerUDPPort);
	m_ServerUDPAddr.sin_addr.s_addr = *((unsigned long*)m_ServerHostEnt->h_addr);

	//start non-blocking, need back to blocking in FD_CONNECT
	//u_long nonBlock = 1;
	//ioctlsocket(Socket, FIONBIO, &nonBlock);

	//create tcp socket
	m_TCPSocket = socket(AF_INET, SOCK_STREAM, 0);
	assert(m_TCPSocket > 0, "create TCPSocket error");

	OnEvent(EventType::BeforeTryConnect);
	int ret = connect(m_TCPSocket, (SOCKADDR*)&m_ServerTCPAddr, LEN_ADDRIN);
	if (ret < 0)
		//fail
	{
		OnEvent(EventType::FailTryConnect);
	}
	else
		//success
	{
		//WSAAsyncSelect
		OnEvent(EventType::SucceedTryConnect);
	}
}

void Client::CheckConnect()
{
	//check is connect successfully
	int ret = IsConnected();
	if ( ret < 0)
		//fail
	{
		OnEvent(EventType::FailConnect);
		closesocket(m_TCPSocket);
	}
	else
		//success
	{
		OnEvent(EventType::SucceedConnect);
	}
}

void Client::SendDisconnect(char* msgBuf)
{
	Send(&m_TCPSocket, msgBuf);
}

void Client::Disconnect()
{
	//close sock
	closesocket(m_TCPSocket);
	closesocket(m_UDPSocket);
	ZeroMemory(&m_ServerTCPAddr, LEN_ADDRIN);
	ZeroMemory(&m_ServerUDPAddr, LEN_ADDRIN);

	//reset val
	m_IP = std::string(DEFAULT_IP);
	m_ServerIP = std::string(DEFAULT_IP);
	m_ServerTCPPort = DEFAULT_PORT;
	m_ServerUDPPort = DEFAULT_PORT + OFFSET_UDP_PORT;
	m_ServerHostEnt = nullptr;
}

int Client::IsConnected()
{
	int addrLen = sizeof(m_ServerTCPAddr);
	return getpeername(m_TCPSocket, (SOCKADDR*)&m_ServerTCPAddr, &addrLen);
}

void Client::AddSendMsg(const char* msgBuf, bool tcp)
{
	if (tcp)
		//tcp
	{
		m_Manager->AddSendMsg(&m_TCPSocket, msgBuf);
	}
	else
		//udp
	{
		m_Manager->AddSendMsg(&m_UDPSocket, msgBuf, &m_ServerUDPAddr);
	}
}
#pragma endregion


