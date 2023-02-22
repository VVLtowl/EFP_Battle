#include "main.h"
#include "ServerBehaviour.h"

#include "Server.h"
#include "NetworkManager.h"

#include "SceneManager.h"

#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "Judgement.h"

#include "imgui/imgui.h"

ServerBehaviour::ServerBehaviour(Server* s, bool isOnce):
	Behaviour(s,isOnce),
	m_OwnerServer(s)
{
}

#pragma region ========== server init ==========

void ServerInit::Start()
{
	//set default val
	{
		m_OwnerServer->m_JoinedClientNum = 0;
		m_OwnerServer->m_TargetClientNum = Judgement::Instance()->Mode.GetPlayerNum();
		m_OwnerServer->m_Port = 5555;
		m_OwnerServer->m_OpenGameRoom = false;
		m_OwnerServer->m_ResetGameRoom = false;
		m_OwnerServer->m_StartGame = false;
	}

	//init network manager
	NetworkManager::Instance()->StartAsServer();


	//test 
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_SERVER_WORK, [this]()
		{
			ImGui::Begin("Server Working...");
			{
				if (m_OwnerServer->m_OpenGameRoom)
				{
					//show port num
					ImGui::Text("port: %d",
						NetworkManager::Instance()->Port);

					//show server address
					ImGui::Text("ip: %s",
						NetworkManager::Instance()->HostIP);

					//show connected clients number
					ImGui::Text("connected clients num: %d",
						m_OwnerServer->m_JoinedClientNum);

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
	
	//next bh start in next scene
	SceneManager::ChangeScene<ServerSettingScene>();
}
#pragma endregion

#pragma region ========== server uninit ==========
void ServerUninit::Start()
{
	//set default val
	{
		m_OwnerServer->m_JoinedClientNum = 0;
		m_OwnerServer->m_TargetClientNum = 0;
		m_OwnerServer->m_OpenGameRoom = false;
		m_OwnerServer->m_ResetGameRoom = false;
		m_OwnerServer->m_StartGame = false;
	}

	if (m_OwnerServer->m_OpenGameRoom)
	{
		m_OwnerServer->m_OpenGameRoom = false;
		NetworkManager::Instance()->Server_CommandDisconnect();
	}

	//close test panel
	DebugInfo::TestBlocks.erase(TESTBLOCKID_SERVER_WORK);
}
#pragma endregion

#pragma region ========== judgement input game room ==========

void ServerInputGameRoom::Start()
{
	//reset value
	m_OwnerServer->m_OpenGameRoom = false;

	//test
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_SERVER_INPUTGAMEROOM, [this]()
		{
			ImGui::Begin("Server Game Room Setting");
			{
				//set player number
				ImGui::Text("target player number: ");
				ImGui::InputInt("num", &m_OwnerServer->m_TargetClientNum);

				//input port
				ImGui::InputInt("port", &m_OwnerServer->m_Port, 0, 65535);

				//confirm setting and start wait client
				if (ImGui::Button("Comfirm & Open Room"))
				{
					m_OwnerServer->m_OpenGameRoom = true;
				}
			}
			ImGui::End();
		});
}

void ServerInputGameRoom::Update()
{
	//test
	if (m_OwnerServer->m_OpenGameRoom)
	{
		//init socket and start asynselect
		NetworkManager::Instance()->TryLaunchServer(m_OwnerServer->m_Port, true);

		//close panel
		DebugInfo::TestBlocks.erase(TESTBLOCKID_SERVER_INPUTGAMEROOM);

		//next to wait client join
		//SwitchToNext(m_OwnerServer->BH_WaitClientsJoin);
		//next
		NextState();
	}
}

#pragma endregion

#pragma region ========== wait clients join server ==========

void ServerWaitClientsJoin::Start()
{
	//reset val
	{
		m_OwnerServer->m_StartGame = false;
		m_OwnerServer->State = (int)Server::State_WaitClientsJoin::WAIT_CLIENT_REQUEST;
	}

	//test
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_SERVER_WAITCLIENTINROOM, [this]()
		{
			ImGui::Begin("Server Wait Client Join");
			{
				//show num
				ImGui::Text("players: %d / %d",
					m_OwnerServer->m_JoinedClientNum,
					m_OwnerServer->m_TargetClientNum);


				int readyCount = 0;
				for (auto idMem : m_OwnerServer->m_ClientMembers)
				{
					ClientMember* mem = idMem.second;
					int id = idMem.first;
					ImGui::Text("%s [%s]",
						std::string("[" + std::to_string(id) + "]" + mem->Name).c_str(),
						mem->Ready ? std::string("ready").c_str() : std::string("wait").c_str());

					if (mem->Ready)
					{
						readyCount++;
					}
				}

				//click to reset room setting
#if 0
				if (ImGui::Button("ResetRoom"))
				{
					if (!m_OwnerServer->ResetGameRoom)
					{
						m_OwnerServer->ResetGameRoom = true;
						return;
					}
				}
#endif

				//check can click start button
				if (readyCount == m_OwnerServer->m_TargetClientNum)
				{
					//click to start game
					if (ImGui::Button("StartGame"))
					{
						if (!m_OwnerServer->m_StartGame)
						{
							m_OwnerServer->m_StartGame = true;
						}
					}
				}
			}
			ImGui::End();
		});
}

void ServerWaitClientsJoin::Update()
{
	switch ((Server::State_WaitClientsJoin)m_OwnerServer->State)
	{
	case Server::State_WaitClientsJoin::WAIT_CLIENT_REQUEST:
	{
		//test reset room
		if (m_OwnerServer->m_ResetGameRoom)
		{
			//reset
			m_OwnerServer->m_ResetGameRoom = false;

			//set state
			m_OwnerServer->State = (int)Server::State_WaitClientsJoin::BACK_TO_INPUT_ROOM;
		}

		//test start game
		if (m_OwnerServer->m_JoinedClientNum == m_OwnerServer->m_TargetClientNum &&
			m_OwnerServer->m_StartGame)
		{
			//reset
			m_OwnerServer->m_StartGame = false;

			//reset client ready to false
			for (auto idMem : m_OwnerServer->m_ClientMembers)
			{
				int id = idMem.first;
				ClientMember* mem = idMem.second;
				m_OwnerServer->SetClientReady(id, false);
			}

			//set state
			m_OwnerServer->State = (int)Server::State_WaitClientsJoin::FINISH_WAIT;
		}
		break;
	}

	case Server::State_WaitClientsJoin::BACK_TO_INPUT_ROOM:
	{
		//close panel
		DebugInfo::TestBlocks.erase(TESTBLOCKID_SERVER_WAITCLIENTINROOM);

		//quit clients
		//todo

		//next to input game room
		//SwitchToNext(m_OwnerServer->BH_InputGameRoom);
		NextState();

		break;
	}

	case Server::State_WaitClientsJoin::FINISH_WAIT:
	{
		//close panel
		DebugInfo::TestBlocks.erase(TESTBLOCKID_SERVER_WAITCLIENTINROOM);

		//finish and set next
		//Judgement::Instance()->StartWaitClientsCreatePlayer();
		//SwitchToNext(nullptr);
		NextState();

		break;
	}
	}
}

#pragma endregion

#pragma region ========== check client join ==========

ServerCheckClientJoin::ServerCheckClientJoin(
	Server* s,
	const int tcpSockID,
	const std::string memName):
	ServerBehaviour(s, true)
{
	m_CheckMember = new ClientMember(tcpSockID, memName);
}

ServerCheckClientJoin::ServerCheckClientJoin(
	Server* s,
	const SOCKADDR_IN& memAddr,
	const std::string memName):
	ServerBehaviour(s,true)
{
	m_CheckMember = new ClientMember(memAddr, memName);
}

void ServerCheckClientJoin::Start()
{
	if (m_OwnerServer->m_TargetClientNum > m_OwnerServer->m_JoinedClientNum)
		//can join
	{
		//register to client member
		m_OwnerServer->RegisterClientMemberAndSetID(m_CheckMember->TCPSocketID, m_CheckMember);

		//agree request and send joined client infos to this client
		NetworkManager::Instance()->Server_AgreeJoin(m_CheckMember);
		NetworkManager::Instance()->Server_SendJoinedClientInfosToClient(m_CheckMember);

		//next to command other client members that new client joined 
		NetworkManager::Instance()->Server_CommandUpdateJoinedNum(m_CheckMember);

		//broadcast show join info after join
		std::string info =
			"- " +
			std::string("[" + std::to_string(m_CheckMember->TCPSocketID) + "]") +
			m_CheckMember->Name +
			" join room";
		DebugInfo::PrintNetMsg(info.c_str());
		NetworkManager::Instance()->Server_BroadcastTestMsgToShow(info.c_str());
	}
	else
	{
		//command client continue to wait
		NetworkManager::Instance()->Server_DisagreeJoin(m_CheckMember);


		//close tcp connect
		if (NetworkManager::Instance()->IsTCP)
		{
			NetworkManager::Instance()->CloseTCPSock(m_CheckMember->TCPSocketID);
		}

		//test
		delete m_CheckMember;
	}
}
#pragma endregion





