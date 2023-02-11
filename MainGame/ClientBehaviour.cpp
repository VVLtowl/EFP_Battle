#include "main.h"
#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "Client.h"
#include "ClientBehaviour.h"

#include "NetworkManager.h"
#include "SceneManager.h"
#include "SerializedTool.h"

#include "imgui/imgui.h"

#include "TransformAnime.h"
#include "ShowUIHelper.h"
#include "Judgement.h"

#include "UIGameStart.h"
#include "UITurnStart.h"
#include "UIRPSStart.h"
#include "UIActStart.h"


ClientBehaviour::ClientBehaviour(Client* c, bool isOnce) :
	m_OwnerClient(c),
	Behaviour(c, isOnce)
{
}


#pragma region ========== init ==========

void ClientInit::Start()
{
	//set default val
	{
		m_OwnerClient->IsConnected = false;
		m_OwnerClient->JoinedClientNum = 0;
		m_OwnerClient->TargetClientNum = 0;
		m_OwnerClient->JoinSuccess = false;
		m_OwnerClient->ServerPort = 5555;
		sprintf_s(m_OwnerClient->Name, "nameless");
		sprintf_s(m_OwnerClient->ServerIP, "127.0.0.1");
	}

	//init network manager
	NetworkManager::Instance()->StartAsClient();

	//test 
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_CLIENT_WORK, [this]()
		{
			ImGui::Begin("Client Working...");
			{
				if (m_OwnerClient->JoinSuccess)
				{
					//show port num
					ImGui::Text("port: %d",
						m_OwnerClient->ServerPort);

					//show server address
					ImGui::Text("ip: %s",
						NetworkManager::Instance()->HostIP);

					//show connected clients number
					ImGui::Text("connected clients num: %d",
						m_OwnerClient->JoinedClientNum);

					//show self id
					ImGui::Text("id: %d",
						m_OwnerClient->ID);

					//show self name
					ImGui::Text("name: %s",
						m_OwnerClient->Name);


					//input name
					ImGui::InputText("Test Msg:\n", m_OwnerClient->Chat, 128);

					//send test msg
					if (ImGui::Button("Send Test Msg"))
					{
						//tip: BH_Init is not a loop behaviour
						//m_OwnerClient->JumpToBH(new ClientSendTestMsg(m_OwnerClient));
						m_OwnerClient->SendChatMsg();
					}
				}
				else
				{
					ImGui::Text("...");
				}
			}
			ImGui::End();
		}
	);

	//next bh start in next scene
	SceneManager::ChangeScene<ClientWaitScene>();
}

#pragma endregion

#pragma region ========== uninit ==========

void ClientUninit::Start()
{
	//set default val
	{
		m_OwnerClient->JoinedClientNum = 0;
		m_OwnerClient->TargetClientNum = 0;
	}

	if(	m_OwnerClient->IsConnected)
	{
		m_OwnerClient->IsConnected = false;
		//NetworkManager::Instance()->Client_RequestDisconnect();
	}

	if (m_OwnerClient->JoinSuccess)
	{
		m_OwnerClient->JoinSuccess = false;
		NetworkManager::Instance()->Client_RequestDisconnect();
	}

	//close test panel
	DebugInfo::TestBlocks.erase(TESTBLOCKID_CLIENT_WORK);
}

#pragma endregion

#pragma region ========== select server ==========

void ClientSelectServer::Start()
{
	//init val
	{
		JoinServer = false;
		m_OwnerClient->State =
			(int)Client::Client::State_SelectServer::SELECT_SERVER;
	}

	//test 
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_CLIENT_SELECTSERVER, [this]()
		{
			ImGui::Begin("Select Server");

			if (m_OwnerClient->State == 
				(int)Client::State_SelectServer::SELECT_SERVER)
			{
				//input port input port
				ImGui::InputInt("Port:", &m_OwnerClient->ServerPort, 0, 65535);

				//input server ip
				ImGui::InputText("RoomServerIP:", m_OwnerClient->ServerIP, 16);

				//input name
				ImGui::InputText("Name:", m_OwnerClient->Name, 64);

				if (ImGui::Button("Join Server"))
				{
					if (!JoinServer)
					{
						JoinServer = true;
					}
				}
			}
			else if (m_OwnerClient->State ==
				(int)Client::State_SelectServer::WAIT_SERVER_CHECK_JOINABLE)
			{
				ImGui::Text("wait server check...");
			}

			ImGui::End();
		});
}

void ClientSelectServer::Update()
{
	switch ((Client::State_SelectServer)m_OwnerClient->State)
	{
	case Client::State_SelectServer::SELECT_SERVER:
	{
		if (JoinServer)
		{
			//set state
			m_OwnerClient->State = 
				(int)Client::State_SelectServer::WAIT_SERVER_CHECK_JOINABLE;

			//init socket and start asynselect
			NetworkManager::Instance()->SetConnectServerInfo(
				m_OwnerClient->ServerPort,
				m_OwnerClient->ServerIP,
				true
			);

			//set up server addr
			SOCKADDR_IN serverAddr;
			memset(&serverAddr, 0, LEN_ADDRIN);
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(m_OwnerClient->ServerPort);
			serverAddr.sin_addr.s_addr =
				*((unsigned long*)NetworkManager::Instance()->HostEnt->h_addr);
			memcpy(&m_OwnerClient->ServerAddr, &serverAddr, LEN_ADDRIN);

			//jump behaviour
			JumpTo(new ClientRequestJoinServer(m_OwnerClient));
		}
		break;
	}

	case Client::State_SelectServer::WAIT_SERVER_CHECK_JOINABLE:
	{

		break;
	}

	case Client::State_SelectServer::FINISH_JOIN:
	{
		//finish
		SwitchToNext(nullptr);

		break;
	}
	}
}

void ClientSelectServer::End()
{
	//reset val
	m_OwnerClient->State =
		(int)Client::Client::State_SelectServer::NONE;
	JoinServer = false;

	//close panel
	DebugInfo::CloseBlock(TESTBLOCKID_CLIENT_SELECTSERVER);
}
#pragma endregion

#pragma region ========== request join server ==========

void ClientRequestJoinServer::Start()
{
	//send to server
	if (NetworkManager::Instance()->IsTCP)
		//tcp
	{
		//start communication after connected
		if (!m_OwnerClient->IsConnected)
		{
			NetworkManager::Instance()->Client_TryConnectServer();
		}
		else
		{
			NetworkManager::Instance()->Client_RequestJoinServer();
		}
	}
	else
		//udp
	{
		NetworkManager::Instance()->Client_RequestJoinServer();
	}
}

#pragma endregion

#pragma region ========== wait all pieces finish ==========

void ClientWaitPiecesFinish::Reset(int targetNum,std::function<void()> endEvent)
{
	if (Working)
	{
		return;
	}

	//reset val
	{
		Working = true;
		PrintRowID = -1;

		SetEndEvent(endEvent);
		
		FinishPieceNum = 0;
		TargetPieceNum= targetNum;
	}
}

void ClientWaitPiecesFinish::PrintCount(std::string info)
{
	if (PrintRowID == -1)
	{
		PrintRowID = DebugInfo::Print(
			"[" +
			std::to_string(FinishPieceNum) +
			"/" +
			std::to_string(TargetPieceNum) +
			"]" +
			info);
	}
	else
	{
		DebugInfo::PrintRow(PrintRowID,
			"[" +
			std::to_string(FinishPieceNum) +
			"/" +
			std::to_string(TargetPieceNum) +
			"]" +
			info);
	}
}

void ClientWaitPiecesFinish::Update()
{
	if (FinishPieceNum == TargetPieceNum)
	{
		//finish 
		NextState();

		//reset working flag
		Working = false;
	}
}

#pragma endregion

#pragma region ========== show step ==========
ClientShowStep::ClientShowStep(Client* c):
	ClientBehaviour(c)
{
	ShowHelper = new ShowUIHelper();
}

ClientShowStep::~ClientShowStep()
{
	delete ShowHelper;
}

void ClientShowStep::Start()
{
	//init anime
	if(ShowHelper->UIObject)
	{
		ShowHelper->Start();
	}
}

void ClientShowStep::Update()
{
	if (ShowHelper->UpdateCheckFinish())
	{
		//delete step ui
		ShowHelper->UIObject->SetState(GameObject::DEAD);

		//finish
		NextState();
	}
}

void ClientShowStep::Reset(int stepType, std::function<void()> endEvent)
{
	switch ((Judgement::ShowStepType)stepType)
	{
	case Judgement::ShowStepType::GAME_START:
	{
		ShowHelper->UIObject = new UIGameStart();
		break;
	}

	case Judgement::ShowStepType::TURN_START:
	{
		//test
		//update piece ui
		for (auto idPiece : m_OwnerClient->Pieces)
		{
			Piece* piece = idPiece.second;
			piece->SetUIHands(false);
			piece->SetUIFinish(false);
			piece->SetUISelect(false);
			piece->SetUIThink(false);
			piece->SetUIActpoint(false);
		}

		UITurnStart* uiTurnStart = new UITurnStart();
		uiTurnStart->SetNumber(m_OwnerClient->TurnCount);
		ShowHelper->UIObject = uiTurnStart;
		break;
	}

	case Judgement::ShowStepType::RPS_START:
	{
		ShowHelper->UIObject = new UIRPSStart();
		break;
	}

	case Judgement::ShowStepType::ACT_START:
	{
		ShowHelper->UIObject = new UIActStart();
		break;
	}

	case Judgement::ShowStepType::TURN_END:
	{

		ShowHelper->UIObject = nullptr;
		break;
	}
	}

	if (ShowHelper->UIObject)
	{
		//dont show at first
		ShowHelper->UIObject->GetTransform()->SetPosition(0, -10000, 0);
	}
	SetEndEvent(endEvent);
}
#pragma endregion

