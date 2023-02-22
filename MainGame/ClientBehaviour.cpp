#include "main.h"
#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "Client.h"
#include "Server.h"
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
#include "UIGameOver.h"

#include <iostream>
#include <thread>

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
		m_OwnerClient->m_IsConnected = false;
		m_OwnerClient->m_JoinSuccess = false;
		m_OwnerClient->m_ServerPort = 5555;
		sprintf_s(m_OwnerClient->m_Name, "nameless");
		sprintf_s(m_OwnerClient->m_ServerIP, "127.0.0.1");
	}

	//init network manager
	NetworkManager::Instance()->StartAsClient();

	//test 
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_CLIENT_WORK, [this]()
		{
			ImGui::Begin("Client Working...");
			{
				if (m_OwnerClient->m_JoinSuccess)
				{
					//show port num
					ImGui::Text("port: %d",
						m_OwnerClient->m_ServerPort);

					//show server address
					ImGui::Text("ip: %s",
						NetworkManager::Instance()->HostIP);

					//show connected clients number
					ImGui::Text("connected clients num: %d",
						m_OwnerClient->m_ClientInfos.size());

					//show self id
					ImGui::Text("id: %d",
						m_OwnerClient->m_ID);

					//show self name
					ImGui::Text("name: %s",
						m_OwnerClient->m_Name);


					//input name
					ImGui::InputText("Test Msg:\n", m_OwnerClient->m_Chat, 128);

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
	if(	m_OwnerClient->m_IsConnected)
	{
		m_OwnerClient->m_IsConnected = false;
		//NetworkManager::Instance()->Client_RequestDisconnect();
	}

	if (m_OwnerClient->m_JoinSuccess)
	{
		m_OwnerClient->m_JoinSuccess = false;
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
		m_JoinServer = false;

		//m_OwnerClient->State =(int)Client::Client::State_SelectServer::SELECT_SERVER;
		m_SelectState = State::SELECT_SERVER;
	}

	//test 
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_CLIENT_SELECTSERVER, [this]()
		{
			ImGui::Begin("Select Server");

			if (m_SelectState == State::SELECT_SERVER)
			{
				//input port input port
				ImGui::InputInt("Port:", &m_OwnerClient->m_ServerPort, 0, 65535);

				//input server ip
				ImGui::InputText("RoomServerIP:", m_OwnerClient->m_ServerIP, 16);

				//input client user name
				ImGui::InputText("Name:", m_OwnerClient->m_Name, 64);

				if (ImGui::Button("Join Server"))
				{
					if (!m_JoinServer)
					{
						m_JoinServer = true;
					}
				}
			}
			else if (m_SelectState == State::TRY_JOIN)
			{
				ImGui::Text("wait server check...");
			}

			ImGui::End();
		});
}

void ClientSelectServer::Update()
{
	switch (m_SelectState)
	{
	case State::SELECT_SERVER:
	{
		PrintUpdateState("select server");

		if (m_JoinServer)
		{
			//init socket and start asynselect
			NetworkManager::Instance()->SetConnectServerInfo(
				m_OwnerClient->m_ServerPort,
				m_OwnerClient->m_ServerIP,
				true);

			//set up server addr
			SOCKADDR_IN serverAddr;
			memset(&serverAddr, 0, LEN_ADDRIN);
			serverAddr.sin_family = AF_INET;
			serverAddr.sin_port = htons(m_OwnerClient->m_ServerPort);
			serverAddr.sin_addr.s_addr =
				*((unsigned long*)NetworkManager::Instance()->HostEnt->h_addr);
			memcpy(&m_OwnerClient->m_ServerAddr, &serverAddr, LEN_ADDRIN);

			//jump behaviour
			//JumpToBH(new ClientRequestJoinServer(m_OwnerClient));
			
			//when connect success, set wait server check and request ID
			m_SelectState = State::TRY_JOIN;
		}
		break;
	}

	case State::TRY_JOIN:
	{
		PrintState("try join");

		//check connect at first or request directly
		m_OwnerClient->TryJoinServer();

		//next to wait
		m_SelectState = State::WAIT_SERVER_CHECK;

		break;
	}

	case State::WAIT_SERVER_CHECK:
	{
		PrintUpdateState("wait server check");
		break;
	}

	case State::FINISH_JOIN:
	{
		//finish
		NextState();

		//close panel
		DebugInfo::CloseBlock(TESTBLOCKID_CLIENT_SELECTSERVER);

		break;
	}
	
	case State::FINISH_EXIT:
	{
		//finish
		NextState();

		//close panel
		DebugInfo::CloseBlock(TESTBLOCKID_CLIENT_SELECTSERVER);

		break;
	}
	}
}


#pragma endregion

#pragma region ========== tcp try connect ==========

void ClientTryConnectServer::Reset(std::function<void()> onSuccess, std::function<void()> onFail, DWORD timeOut)
{
	//set time out
	m_TimeOut = timeOut;

	//set up event
	OnConnectSuccess = onSuccess;
	OnConnectFail = onFail;

	if (NetworkManager::Instance()->IsTCP)
		//tcp try connect server
	{
		//start communication after connected
		if (!m_OwnerClient->m_IsConnected)
		{
			//reset time count
			m_TimeStamp = GetTickCount();

			//non-blocking connect
			NetworkManager::Instance()->Client_TryConnectServer();
			//std::thread thread1(&NetworkManager::Client_TryConnectServer, NetworkManager::Instance());
			//thread1.detach();

			//set state
			m_ConnectState = State::WAIT_CONNECT;
		}
		else
		{
			//todo
			//set state
			m_ConnectState = State::NONE;
		}
	}
	else
		//udp dont need connect
	{
		//todo
		//set state
		m_ConnectState = State::NONE;
	}
}

void ClientTryConnectServer::Update()
{
	switch (m_ConnectState)
	{
	case State::WAIT_CONNECT:
	{
		m_TimeCount = GetTickCount() - m_TimeStamp;
		PrintUpdateState("wait connect(" + std::to_string(m_TimeCount) + ")");

		break;
	}

	case State::CHECK_CONNECT:
	{
		PrintState("check connect");

		//use time out to check connect successfully
		m_TimeCount = GetTickCount()-m_TimeStamp;
		if (m_TimeCount < m_TimeOut)
		{
			DebugInfo::Print("connect success");

			//event
			OnConnectSuccess();
			
			//connect
			m_OwnerClient->m_IsConnected = true;

			//set state for check end event
			m_ConnectState = State::CONNECT_SUCCEED;
		}
		else
		{
			DebugInfo::Print("connect time out");

			//event
			OnConnectFail(); 

			//set state for check end event
			m_ConnectState = State::CONNECT_TIME_OUT;
		}

		//finish
		NextState();
		break;
	}
	}
}

#pragma endregion

#pragma region ========== request join server ==========

void ClientRequestJoinServer::Start()
{
	//request server check joinable
	if (NetworkManager::Instance()->IsTCP)
		//tcp
	{
		//start communication after connected
		if (m_OwnerClient->m_IsConnected)
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

#pragma region ========== wait room ==========

void ClientWaitRoom::Start()
{
	//reset val
	{
		m_Ready = false;
		m_WaitState = State::INPUT_READY;
	}

	//test
	//start room info panel
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_CLIENT_WAITROOM, [this]()
		{
			ImGui::Begin("Wait Room");
			{
				//show waiting clients num
				ImGui::Text("num of player: %d / %d",
					m_OwnerClient->m_ClientInfos.size(),
					m_OwnerClient->m_TargetClientNum);
				ImGui::Text("");

				//show player ready
				for (auto idMem : m_OwnerClient->m_ClientInfos)
				{
					int id = idMem.first;
					ClientMember* mem = idMem.second;
					ImGui::Text("%s [%s]",
						std::string("[" + std::to_string(id) + "]" + mem->Name).c_str(),
						mem->Ready ? std::string("ready").c_str() : std::string("wait").c_str());
				}

				//set ready
				ImGui::Text("-------------------------");
				if (m_WaitState == ClientWaitRoom::State::INPUT_READY)
				{
					if (ImGui::Button("Ready"))
					{
						if (!m_Ready)
						{
							m_Ready = true;
						}
					}
				}
				else if(m_WaitState == ClientWaitRoom::State::WAIT_START)
				{
					ImGui::Text("wait game start...");
				}
			}
			ImGui::End();
		});
}

void ClientWaitRoom::Update()
{
	switch (m_WaitState)
	{
	case State::INPUT_READY:
	{
		PrintUpdateState("input ready");

		if (m_Ready)
		{
			NetworkManager::Instance()->Client_RequestSetReady(true);

			//when connect success, set wait server check and request ID
			m_WaitState = State::WAIT_START;
		}
		break;
	}

	case State::WAIT_START:
	{
		PrintUpdateState("wait start");
		break;
	}

	case State::FINISH:
	{
		DebugInfo::TestBlocks.erase(TESTBLOCKID_CLIENT_WAITROOM);
		NextState();
		break;
	}
	}
}
#pragma endregion




#pragma region ========== wait all pieces finish ==========

void ClientWaitPiecesFinish::Reset(int targetNum, std::string endIndo, std::function<void()> endEvent)
{
	if (Working)
	{
		return;
	}

	//reset val
	{
		Working = true;
		PrintRowID = -1;

		SetEndEvent(endIndo,endEvent);
		
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
	if(ShowHelper->m_UIObject)
	{
		ShowHelper->Start();
	}
}

void ClientShowStep::Update()
{
	if (ShowHelper->UpdateCheckFinish())
	{
		//delete step ui
		ShowHelper->m_UIObject->SetState(GameObject::DEAD);

		//finish
		NextState();
	}
}

void ClientShowStep::Reset(int stepType, std::string endInfo, std::function<void()> endEvent)
{
	switch ((Judgement::ShowStepType)stepType)
	{
	case Judgement::ShowStepType::GAME_START:
	{
		ShowHelper->m_UIObject = new UIGameStart();
		break;
	}

	case Judgement::ShowStepType::TURN_START:
	{
		//test
		//update piece ui
		for (auto idPiece : GameManager::m_Pieces)
		{
			Piece* piece = idPiece.second;
			piece->SetUIHands(false);
			piece->SetUIFinish(false);
			piece->SetUISelect(false);
			piece->SetUIThink(false);
			piece->SetUIActpoint(false);
		}

		UITurnStart* uiTurnStart = new UITurnStart();
		uiTurnStart->SetNumber(m_OwnerClient->m_TurnCount);
		ShowHelper->m_UIObject = uiTurnStart;
		break;
	}

	case Judgement::ShowStepType::RPS_START:
	{
		ShowHelper->m_UIObject = new UIRPSStart();
		break;
	}

	case Judgement::ShowStepType::ACT_START:
	{
		ShowHelper->m_UIObject = new UIActStart();
		break;
	}

	case Judgement::ShowStepType::TURN_END:
	{

		ShowHelper->m_UIObject = nullptr;
		break;
	}
	}

	if (ShowHelper->m_UIObject)
	{
		//dont show at first
		ShowHelper->m_UIObject->GetTransform()->SetPosition(0, -10000, 0);
	}
	SetEndEvent(endInfo, endEvent);
}
#pragma endregion

#pragma region ========== show game over ==========

ClientShowGameOver::ClientShowGameOver(Client* c):
	ClientBehaviour(c)
{
	m_ShowHelper = new ShowGameOverUIHelper();
}
ClientShowGameOver::~ClientShowGameOver()
{
	delete m_ShowHelper;
}
void ClientShowGameOver::Start()
{
	//init anime
	if (m_ShowHelper->m_UIObject)
	{
		m_ShowHelper->Start();
	}
}
void ClientShowGameOver::Update()
{
	if (m_ShowHelper->UpdateCheckFinish())
	{
		//delete game over ui
		m_ShowHelper->m_UIObject->SetState(GameObject::DEAD);

		//finish
		NextState();
	}
}
void ClientShowGameOver::Reset(ResultType result, std::string endInfo, std::function<void()> endEvent)
{
	//set game over ui
	switch (result)
	{
	case ResultType::BAD_WIN:
	{
		m_ShowHelper->m_UIObject = new UIGameOver(UIGameOver::ResultType::BAD_WIN);
		break;
	}

	case ResultType::GOOD_WIN:
	{
		m_ShowHelper->m_UIObject = new UIGameOver(UIGameOver::ResultType::GOOD_WIN);
		break;
	}
	}

	if (m_ShowHelper->m_UIObject)
	{
		//dont show at first
		m_ShowHelper->m_UIObject->GetTransform()->SetPosition(0, -10000, 0);
	}
	SetEndEvent(endInfo, endEvent);
}
#pragma endregion
