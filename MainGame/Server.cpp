#include "main.h"
#include "Judgement.h"
#include "ServerBehaviour.h"
#include "MyNetManager.h"
#include "Server.h"

#include "imgui/imgui.h"

AppServer::AppServer()
{
	//create server (server_as)
	m_MyServer = new MyServer(MyNetManager::Instance());
	MyNetManager::Instance()->m_TargetAppServer = this; 

	//reset room data
	m_JoinedClientNum = 0;
	m_TargetClientNum = 0;
	
	//reset button
	m_OpenGameRoom = false;
	m_ResetGameRoom = false;
	m_StartGame = false;

	//create behaviour
	//BH_Init = new ServerInit(this);
	//BH_Uninit = new ServerUninit(this);
	BH_InputGameRoom = new ServerInputGameRoom(this);
	BH_WaitClientsJoin = new ServerWaitClientsJoin(this);
}

AppServer::~AppServer()
{
	m_ClientMembers.clear();
	delete m_MyServer;
	MyNetManager::Instance()->m_TargetAppServer = nullptr;
}

#pragma region ========== behaviour ==========

void AppServer::Init()
{
	//set default val
	{
		m_JoinedClientNum = 0;
		m_TargetClientNum = Judgement::Instance()->m_Mode.GetPlayerNum();
		m_Port = 5555;
		m_OpenGameRoom = false;
		m_ResetGameRoom = false;
		m_StartGame = false;
	}

	//test 
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_SERVER_WORK, [this]()
		{
			ImGui::Begin("Server Working...");
			{
				if (m_OpenGameRoom)
				{
					//show port num
					ImGui::Text("port: %d",
						m_MyServer->m_TCPPort);

					//show server address
					ImGui::Text("ip: %s",
						m_MyServer->m_IP.c_str());

					//show connected clients number
					ImGui::Text("connected clients num: %d",
						m_JoinedClientNum);

					//test for chat
					//todo
				}
				else
				{
					ImGui::Text("...");
				}
			}
			ImGui::End();
		});
}

void AppServer::Uninit()
{
	//close test panel
	DebugInfo::TestBlocks.erase(TESTBLOCKID_SERVER_WORK);

	//set default val
	{
		m_JoinedClientNum = 0;
		m_TargetClientNum = 0;
		m_OpenGameRoom = false;
		m_ResetGameRoom = false;
		m_StartGame = false;
	}

	if (m_OpenGameRoom)
	{
		m_OpenGameRoom = false;
		GetNetSendFunc().Server_CommandDisconnect();
	}

}

void AppServer::ClearMember()
{
	while (m_ClientMembers.empty() == false)
	{
		auto idMem = m_ClientMembers.begin();
		delete (*idMem).second;
		m_ClientMembers.erase((*idMem).first);
	}
	m_ClientMembers.clear();
}

void AppServer::StartInWaitRoomScene()
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

void AppServer::StartInputGameRoom()
{
	BH_InputGameRoom->Reset();
	BH_InputGameRoom->SetEndEvent("StartWaitClientsJoin", 
		[this]()
		{
			StartWaitClientsJoin();
		});
	StartBH(BH_InputGameRoom);
}

void AppServer::LaunchMyServer()
{
	//start server
	m_MyServer->Launch();
	m_MyServer->Bind();
	m_MyServer->Listen();
}

void AppServer::StartWaitClientsJoin()
{
	BH_WaitClientsJoin->Reset();
	BH_WaitClientsJoin->SetEndEvent("StartWaitClientsCreatePlayer", 
		[this]()
		{
			if (BH_WaitClientsJoin->m_WaitState == 
				ServerWaitClientsJoin::State::FINISH_WAIT)
			{
				StartBH(nullptr);

				//judgement create players and pieces
				Judgement::Instance()->ShuffleAndSetPiecesToPlayers();
				Judgement::Instance()->SetPlayerClient();
				Judgement::Instance()->StartWaitClientsCreatePlayer();
			}
			else if (BH_WaitClientsJoin->m_WaitState == 
				ServerWaitClientsJoin::State::BACK_TO_INPUT_ROOM)
			{
				StartInputGameRoom();
			}
		});
	StartBH(BH_WaitClientsJoin);
}

void AppServer::StartCheckClientJoin(int tcpID, std::string newClientName)
{
	DebugInfo::Print("check new client[" + newClientName + "] join");

	//use tcp
	JumpToBH(new ServerCheckClientJoin(
		this,
		tcpID, 
		newClientName));
}

void AppServer::SetClientDisconnect(int id)
{
	//copy! for send msg
	ClientMember disconnectClient = *m_ClientMembers[id];
	disconnectClient.Disconnect = true;

	//delete
	delete m_ClientMembers[id];
	m_ClientMembers.erase(id);

	//close tcp sock
	m_MyServer->FreeClientID(id);



	//set quit info before close and quit
	std::string info =
		"- " +
		std::string("[" + std::to_string(id) + "]") +
		disconnectClient.Name +
		" quit room";
	DebugInfo::PrintNetMsg(info.c_str());

	//update client members and notify other clients
	GetNetSendFunc().Server_CommandUpdateJoinedNum(&disconnectClient);
	GetNetSendFunc().Server_BroadcastTestMsgToShow(info.c_str());
}

void AppServer::SetClientReady(int id,bool ready)
{
	DebugInfo::Print(
		"set client[" +
		std::to_string(id) +
		"] ready [" +
		(ready ? "true" : "false") +
		"]");

	ClientMember& readyClient = *m_ClientMembers[id];
	readyClient.Ready = ready;

	//command all clients update client info
	GetNetSendFunc().Server_CommandUpdateJoinedNum(&readyClient);
}

#pragma endregion