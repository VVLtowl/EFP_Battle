#pragma once
#include "NetworkObject.h"

#pragma region ========== manager ==========
/*********************************************************
* @brief    for send and recv by queue
********************************************************/
class TransportMsg
{
public:
	TransportMsg(SOCKET* socket,
		const char* msg,
		SOCKADDR_IN* addrin = nullptr);
public:
	SOCKET* m_Socket;
	SOCKADDR_IN* m_UDPAddr;
	char m_MsgBuffer[LEN_MSG];
};
/*********************************************************
* @brief	network manager (base)
********************************************************/
class NetworkManager
{
	friend class NetworkObject;
public:
	void Update();
	void SetRecvProcess(std::function<void(TransportMsg* recvMsg)> recvFunc = [](TransportMsg* msg) {});
	void SetSendProcess(std::function<void(TransportMsg* sendMsg)> sendFunc = [](TransportMsg* msg) {});
	//void ClearQueue();//only in shut down

	void AddRecvMsg(SOCKET* socket,
		const char* msg,
		SOCKADDR_IN* addrin = nullptr);
	void AddSendMsg(SOCKET* socket,
		const char* msg,
		SOCKADDR_IN* addrin = nullptr);

protected:
	NetworkObject* m_NetworkObj;
	std::queue<TransportMsg*> m_RecvMsgQueue;
	std::queue<TransportMsg*> m_SendMsgQueue;
	std::function<void(TransportMsg* recvMsg)> m_RecvProcess;
	std::function<void(TransportMsg* sendMsg)> m_SendProcess;
};
#pragma endregion

