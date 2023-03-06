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

#pragma region ========== common msg structure ==========
TransportMsg::TransportMsg(SOCKET* socket, const char* msg, SOCKADDR_IN* addrin)
{
	m_Socket = socket;
	m_UDPAddr = addrin;
	memcpy(m_MsgBuffer, msg, LEN_MSG);
}
#pragma endregion

#pragma region ========== network manager (base) ==========
void NetworkManager::SetRecvProcess(std::function<void(TransportMsg* recvMsg)> recvFunc)
{
	m_RecvProcess = recvFunc;
}

void NetworkManager::SetSendProcess(std::function<void(TransportMsg* recvMsg)> sendFunc)
{
	m_SendProcess = sendFunc;
}


void NetworkManager::AddRecvMsg(
	SOCKET* socket,
	const char* msgBuf,
	SOCKADDR_IN* addrin)
{
	m_RecvMsgQueue.push(new TransportMsg(socket, msgBuf, addrin));
}

void NetworkManager::AddSendMsg(
	SOCKET* socket,
	const char* msgBuf,
	SOCKADDR_IN* addrin)
{
	m_SendMsgQueue.push(new TransportMsg(socket, msgBuf, addrin));
}


void NetworkManager::Update()
{
	//check recv
	while (!m_RecvMsgQueue.empty())
	{
		TransportMsg* recvMsg = m_RecvMsgQueue.front();
		m_RecvProcess(recvMsg);
		m_RecvMsgQueue.pop();
		delete recvMsg;
	}

	//check send
	while (!m_SendMsgQueue.empty())
	{
		TransportMsg* sendMsg = m_SendMsgQueue.front();
		m_SendProcess(sendMsg);
		m_SendMsgQueue.pop();
		delete sendMsg;
	}
}
#pragma endregion
