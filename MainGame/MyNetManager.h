#pragma once
#include "network/NetworkObject.h"
#include "network/NetworkManager.h"
#include "Singleton.h"
#include "NetworkSendFunc.h"

class NetworkManager_AS;

/*********************************************************
* @brief	for async select
********************************************************/
//interface
class NetworkUpdateProc
{
public:
	NetworkUpdateProc(class NetworkManager_AS* owner);
	virtual ~NetworkUpdateProc();
	virtual void UpdateNetProc(UINT uWinMsg, LPARAM lParam) = 0;

protected:
	class NetworkManager_AS* m_ASManager;
};

//async select server
class Server_AS :
	public Server,
	public NetworkUpdateProc
{
public:
	Server_AS(class NetworkManager_AS* owner);
	void UpdateNetProc(UINT uWinMsg, LPARAM lParam)override;
};

//async select client
class Client_AS :
	public Client,
	public NetworkUpdateProc
{
public:
	Client_AS(class NetworkManager_AS* owner);
	void UpdateNetProc(UINT uWinMsg, LPARAM lParam)override;
};

//async select network manager
class NetworkManager_AS :
	public NetworkManager
{
	friend class NetworkUpdateProc;
public:
	void UpdateNetProc(UINT uWinMsg, LPARAM lParam);
public:
	NetworkUpdateProc* m_NetworkUpdateProcObj;
};

/*********************************************************
* @brief	my net manager
********************************************************/
class SendOperate :
	public TransportMsg
{
public:
	SendOperate(SOCKET* sock,
		const char* msg,
		SOCKADDR_IN* addrin = nullptr,
		int rowID = -1, 
		std::string destInfo="",
		std::string commend = "") :
		TransportMsg(sock, msg, addrin),
		m_PrintRowID(rowID),
		m_DestInfo(destInfo),
		m_Comment(commend) {};
public:
	int m_PrintRowID;
	std::string m_DestInfo;
	std::string m_Comment;
};

class MyServer :
	public Server_AS
{
public:
	MyServer(class MyNetManager* owner);
	void SendToOne(int clientID,const MsgContent& content, std::string comment);
	void SendToAll(const MsgContent& content, std::string comment);
	void SendToOther(int exceptClientID, const MsgContent& content, std::string comment);

private:
	class MyNetManager* m_MyNetManager;
};

class MyClient :
	public Client_AS
{
public:
	MyClient(class MyNetManager* owner);
	void SendToServer(const MsgContent& content, std::string comment);

private:
	class MyNetManager* m_MyNetManager;
};

class MyNetManager :
	public NetworkManager_AS,
	public Singleton<MyNetManager>
{
public:
	void AddSendMsg(SendOperate* sendOp);
	void SetChatPanel(bool show);
	class AppClient* m_TargetAppClient; 
	class AppServer* m_TargetAppServer;
};