#include "main.h"
#include "Server.h"
#include "Judgement.h"
#include "ServerBehaviour.h"
#include "NetworkManager.h"

Server::Server()
{
	//reset room data
	m_JoinedClientNum = 0;
	m_TargetClientNum = 0;
	
	//reset button
	m_OpenGameRoom = false;
	m_ResetGameRoom = false;
	m_StartGame = false;

	//create behaviour
	BH_Init = new ServerInit(this);
	BH_Uninit = new ServerUninit(this);
	BH_InputGameRoom = new ServerInputGameRoom(this);
	BH_WaitClientsJoin = new ServerWaitClientsJoin(this);
}

Server::~Server()
{
	//delete behaviour by base class destructor
}


#pragma region ========== manage client member ==========

ClientMember::ClientMember(const SOCKADDR_IN& addr, const std::string& name) :
	Name(name),
	Ready(false)
{
	//constructor set
	memcpy(&Address, &addr, LEN_ADDRIN);

	//wait to set id
	//ID = -1;

	//useless in udp
	TCPSocketID = -1;
}

ClientMember::ClientMember(const int tcpSockID, const std::string& name):
	TCPSocketID(tcpSockID),
	Name(name),
	Ready(false)
{
	//wait to set id
	//ID = -1;

	//useless in tcp
	memset(&Address, 0, LEN_ADDRIN);
}

void Server::RegisterClientMemberAndSetID(int id, ClientMember* c)
{
	m_ClientMembers.emplace(id, c);

	//set client id
	c->TCPSocketID = id;

	m_JoinedClientNum++;
}

void Server::QuitClientMember(ClientMember* c)
{
	m_ClientMembers.erase(c->TCPSocketID);
		
	m_JoinedClientNum--;
}

void Server::QuitClientMember(int id)
{
	m_ClientMembers.erase(id);

	m_JoinedClientNum--;
}

void Server::ClearClientMembers()
{
	for (auto mem : m_ClientMembers)
	{
		delete mem.second;
	}
	m_ClientMembers.clear();
}


#pragma endregion

#pragma region ========== behaviour ==========

void Server::StartInWaitRoomScene()
{
	if (m_OpenGameRoom)
	{
		StartWaitClientsJoin();
	}
	else
	{
		StartInputGameRoom();
	}
}

void Server::StartInputGameRoom()
{
	BH_InputGameRoom->Reset();
	BH_InputGameRoom->SetEndEvent("StartWaitClientsJoin", 
		[this]()
		{
			StartWaitClientsJoin();
		});
	StartBH(BH_InputGameRoom);
}

void Server::StartWaitClientsJoin()
{
	BH_WaitClientsJoin->Reset();
	BH_WaitClientsJoin->SetEndEvent("StartWaitClientsCreatePlayer", 
		[this]()
		{
			if (State == (int)State_WaitClientsJoin::FINISH_WAIT)
			{
				StartBH(nullptr);

				//judgement create players and pieces
				Judgement::Instance()->ShuffleAndSetPiecesToPlayers();
				Judgement::Instance()->StartWaitClientsCreatePlayer();
			}
			else if (State == (int)State_WaitClientsJoin::BACK_TO_INPUT_ROOM)
			{
				StartInputGameRoom();
			}
		});
	StartBH(BH_WaitClientsJoin);
}

void Server::StartCheckClientJoin(int tcpID, std::string newClientName)
{
	DebugInfo::Print("check new client[" + newClientName + "] join");

	if (NetworkManager::Instance()->IsTCP)
	{
		JumpToBH(new ServerCheckClientJoin(
			this, tcpID, newClientName));
	}
}

void Server::SetClientDisconnect(int id)
{
	ClientMember* disconnectClient = m_ClientMembers[id];

	//set quit info before close and quit
	std::string info =
		"- " +
		std::string("[" + std::to_string(disconnectClient->TCPSocketID) + "]") +
		disconnectClient->Name +
		" quit room";
	DebugInfo::PrintNetMsg(info.c_str());

	//close tcp sock
	if (NetworkManager::Instance()->IsTCP)
	{
		int tcpSockID = disconnectClient->TCPSocketID;
		NetworkManager::Instance()->CloseTCPSock(tcpSockID);
	}

	//update client members and notify other clients
	QuitClientMember(id);
	NetworkManager::Instance()->Server_CommandUpdateJoinedNum(disconnectClient);
	NetworkManager::Instance()->Server_BroadcastTestMsgToShow(info.c_str());
}

void Server::SetClientReady(int id,bool ready)
{
	DebugInfo::Print(
		"set client[" +
		std::to_string(id) +
		"] ready [" +
		(ready ? "true" : "false") +
		"]");

	ClientMember* readyClient = m_ClientMembers[id];
	readyClient->Ready = ready;

	//command all clients update client info
	NetworkManager::Instance()->Server_CommandUpdateJoinedNum(readyClient);
}

#pragma endregion