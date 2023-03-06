#pragma once

#include <unordered_map>
#include <functional>
#include <list>
#include <queue>


#pragma region ========== definition ==========
//for limit tcp sock num
const size_t MAX_CLIENT_NUM = 100;

//for async select
#define NET_MSG (WM_USER + 1)
#define OFFSET_NET_MSG_TCP (2)
#define OFFSET_NET_MSG_LISTEN (1)
#define OFFSET_NET_MSG_UDP (0)
#define OFFSET_NET_MSG_MAX (MAX_CLIENT_NUM+OFFSET_NET_MSG_TCP)

//network object default value 
const size_t LEN_IP = 16;//000.000.000.000x
const char DEFAULT_IP[LEN_IP] = "127.0.0.1";
const int DEFAULT_PORT = 5555;
const int OFFSET_UDP_PORT = 1;
const size_t LEN_ADDRIN = sizeof(SOCKADDR_IN);

//send type
const bool SEND_BY_TCP = true;
const bool SEND_BY_UDP = !SEND_BY_TCP;

//net package definition
const size_t LEN_MSG = 1024;
#pragma endregion

#pragma region ========== network object ==========
/*********************************************************
* @brief	networkí êMëŒè€(base class)
********************************************************/
class NetworkObject
{
public:
	NetworkObject(class NetworkManager* owner);
	virtual ~NetworkObject();

	//necessary
	virtual void Launch();
	virtual void ShutDown();

	//unnecessary
	virtual void Bind() {};//server
	virtual void Listen() {};//server
	virtual void TryConnect() {};//client
	virtual void CheckConnect() {};//client
	virtual void Accept() {};//server

protected:
	void Recv(SOCKET* socket);
	int Send(SOCKET* socket,char* msgBuf);
	void RecvFrom(SOCKET* socket, SOCKADDR_IN* from);
	int SendTo(SOCKET* socket, char* msgBuf, SOCKADDR_IN* dest);

protected:
	WSADATA m_WsaData;
	class NetworkManager* m_Manager;
public:
	bool m_StartUp = false;

	/*********************************************************
	* @brief	event
	********************************************************/
public:
	enum class EventType
	{
		SucceedLaunch,
		FailLaunch,
		SucceedLaunchServer,
		SucceedLaunchClient,

		BeforeShutDown,
		SucceedShutDown,
		FailShutDown,

		SucceedBindTCPListen,
		FailBindTCP,
		SucceedBindUDP,
		FailBindUDP,

		SucceedStartListen,
		FailStartListen,

		BeforeTryConnect,
		SucceedTryConnect,
		FailTryConnect,
		SucceedConnect,
		FailConnect,


		CannotAccept,
		SucceedAccept,
		FailAccept,

		Max,
	};
	void AddEvent(EventType type, std::function<void(int arg)> eventFunc);
	void ClearEvent(EventType type);
protected:
	std::list<std::function<void(int arg)>> m_Events[(int)EventType::Max];
	void OnEvent(EventType type, int arg = 0);
};

/*********************************************************
* @brief	Socket
********************************************************/
class Server :
	public NetworkObject
{
public:
	Server(class NetworkManager* owner) :NetworkObject(owner) {};

	virtual void Launch()override;
	virtual void Bind()override;
	virtual void Listen()override;
	virtual void Accept()override;

	void Reset( char* msgBuf);

protected:
	virtual void AddSendMsgsToAll(const char* msgBuf, bool tcp);
	//virtual void AddSendMsgsToOther(const char* msgBuf, bool tcp, int exceptID);

	/*********************************************************
	* @brief	server info
	********************************************************/
public:
	std::string m_IP = std::string(DEFAULT_IP);
	int m_TCPPort = DEFAULT_PORT;
	int m_UDPPort = DEFAULT_PORT + OFFSET_UDP_PORT;
	HOSTENT* m_HostEnt = nullptr;

	/*********************************************************
	* @brief	socket
	********************************************************/
protected:
	SOCKET m_UDPSocket;
	SOCKET m_TCPListenSocket;

	std::unordered_map<int,SOCKADDR_IN> m_UDPAddrs;
	std::unordered_map<int,SOCKET> m_TCPSockets;

	bool m_ClientIDs[MAX_CLIENT_NUM];
public:
	int GetEnableClientID();
	void FreeClientID(int id);
};

/*********************************************************
* @brief	Client
********************************************************/
class Client :
	public NetworkObject
{
public:
	Client(class NetworkManager* owner) :NetworkObject(owner) {};

	virtual void Launch()override;
	virtual void TryConnect()override;
	virtual void CheckConnect()override;

	void SendDisconnect(char* msgBuf);
	void Disconnect();
	int IsConnected();
	virtual void AddSendMsg(const char* msgBuf, bool tcp);

	/*********************************************************
	* @brief	client info
	********************************************************/
public:
	std::string m_IP = std::string(DEFAULT_IP);
	std::string m_ServerIP = std::string(DEFAULT_IP);
	int m_ServerTCPPort = DEFAULT_PORT;
	int m_ServerUDPPort = DEFAULT_PORT + OFFSET_UDP_PORT;
	HOSTENT* m_ServerHostEnt = nullptr;

	/*********************************************************
	* @brief	socket
	********************************************************/
public:
	SOCKET m_UDPSocket;
	SOCKET m_TCPSocket;
	SOCKADDR_IN m_ServerTCPAddr;
	SOCKADDR_IN m_ServerUDPAddr;

	int m_ClientID;
};

#pragma endregion

