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
		m_OwnerServer->JoinedClientNum = 0;
		m_OwnerServer->TargetClientNum = Judgement::Instance()->Mode.GetPlayerNum();
		m_OwnerServer->Port = 5555;
		m_OwnerServer->OpenGameRoom = false;
		m_OwnerServer->ResetGameRoom = false;
		m_OwnerServer->StartGame = false;
	}

	//init network manager
	NetworkManager::Instance()->StartAsServer();


	//test 
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_SERVER_WORK, [this]()
		{
			ImGui::Begin("Server Working...");
			{
				if (m_OwnerServer->OpenGameRoom)
				{
					//show port num
					ImGui::Text("port: %d",
						NetworkManager::Instance()->Port);

					//show server address
					ImGui::Text("ip: %s",
						NetworkManager::Instance()->HostIP);

					//show connected clients number
					ImGui::Text("connected clients num: %d",
						m_OwnerServer->JoinedClientNum);

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
		m_OwnerServer->JoinedClientNum = 0;
		m_OwnerServer->TargetClientNum = 0;
		m_OwnerServer->OpenGameRoom = false;
		m_OwnerServer->ResetGameRoom = false;
		m_OwnerServer->StartGame = false;
	}

	if (m_OwnerServer->OpenGameRoom)
	{
		m_OwnerServer->OpenGameRoom = false;
		NetworkManager::Instance()->Server_CommandDisconnect();
	}

	//close test panel
	DebugInfo::TestBlocks.erase(TESTBLOCKID_SERVER_WORK);
}
#pragma endregion

#pragma region ========== judgement input game room ==========

void ServerInputGameRoom::Start()
{
	//test
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_SERVER_INPUTGAMEROOM, [this]()
		{
			ImGui::Begin("Server Game Room Setting");
			{
				//set player number
				ImGui::Text("target player number: ");
				ImGui::InputInt("num", &m_OwnerServer->TargetClientNum);

				//input port
				ImGui::InputInt("port", &m_OwnerServer->Port, 0, 65535);

				//confirm setting and start wait client
				if (ImGui::Button("Comfirm & Open Room"))
				{
					m_OwnerServer->OpenGameRoom = true;
				}
			}
			ImGui::End();
		});
}

void ServerInputGameRoom::Update()
{
	//test
	if (m_OwnerServer->OpenGameRoom)
	{
		//init socket and start asynselect
		NetworkManager::Instance()->TryLaunchServer(m_OwnerServer->Port, true);

		//close panel
		DebugInfo::TestBlocks.erase(TESTBLOCKID_SERVER_INPUTGAMEROOM);

		//next to wait client join
		SwitchToNext(m_OwnerServer->BH_WaitClientsJoin);
	}
}

#pragma endregion

#pragma region ========== wait clients join server ==========

void ServerWaitClientsJoin::Start()
{
	//reset val
	{
		m_OwnerServer->StartGame = false;
		m_OwnerServer->State = (int)Server::State_WaitClientsJoin::WAIT_CLIENT_REQUEST;
	}

	//test
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_SERVER_WAITCLIENTINROOM, [this]()
		{
			ImGui::Begin("Server Wait Client Joint");
			{
				//show num
				ImGui::Text("players: %d / %d",
					m_OwnerServer->JoinedClientNum,
					m_OwnerServer->TargetClientNum);


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
				if (m_OwnerServer->JoinedClientNum ==
					m_OwnerServer->TargetClientNum)
				{
					//click to start game
					if (ImGui::Button("StartGame"))
					{
						if (!m_OwnerServer->StartGame)
						{
							m_OwnerServer->StartGame = true;
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
		if (m_OwnerServer->ResetGameRoom)
		{
			//reset
			m_OwnerServer->ResetGameRoom = false;

			//set state
			m_OwnerServer->State = (int)Server::State_WaitClientsJoin::BACK_TO_INPUT_ROOM;
		}

		//test start game
		if (m_OwnerServer->JoinedClientNum == m_OwnerServer->TargetClientNum &&
			m_OwnerServer->StartGame)
		{
			//reset
			m_OwnerServer->StartGame = false;

			//judgement create players and pieces
			Judgement::Instance()->ShuffleAndSetPiecesToPlayers();

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
		SwitchToNext(m_OwnerServer->BH_InputGameRoom);

		break;
	}

	case Server::State_WaitClientsJoin::FINISH_WAIT:
	{
		//close panel
		DebugInfo::TestBlocks.erase(TESTBLOCKID_SERVER_WAITCLIENTINROOM);

		//finish and set next
		SwitchToNext(nullptr);
		Judgement::Instance()->StartWaitClientsCreatePlayer();

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
	CheckMember = new ClientMember(tcpSockID, memName);
}

ServerCheckClientJoin::ServerCheckClientJoin(
	Server* s,
	const SOCKADDR_IN& memAddr,
	const std::string memName):
	ServerBehaviour(s,true)
{
	CheckMember = new ClientMember(memAddr, memName);
}

void ServerCheckClientJoin::Start()
{
	if (m_OwnerServer->TargetClientNum > m_OwnerServer->JoinedClientNum)
		//can join
	{
		//register to client member
		m_OwnerServer->JoinedClientNum++;
		m_OwnerServer->RegisterClientMemberAndSetID(CheckMember);

		//agree request
		NetworkManager::Instance()->Server_AgreeJoin(CheckMember);

		//next to command other client members that new client joined 
		NetworkManager::Instance()->Server_CommandUpdateJoinedNum();

		//broadcast show join info after join
		std::string info =
			"- " +
			std::string("[" + std::to_string(CheckMember->ID) + "]") +
			CheckMember->Name +
			" join room";
		DebugInfo::PrintNetMsg(info.c_str());
		NetworkManager::Instance()->Server_BroadcastTestMsgToShow(info.c_str());
	}
	else
	{
		//test
		delete CheckMember;

		//command client wait
		//todo
	}
}
#pragma endregion





