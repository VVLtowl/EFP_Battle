#include "main.h"

#include "MyNetManager.h"
#include "SceneManager.h"

#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "Judgement.h"

#include "Server.h"
#include "ServerBehaviour.h"

#include "imgui/imgui.h"

ServerBehaviour::ServerBehaviour(AppServer* s, bool isOnce):
	Behaviour(s,isOnce),
	m_AppServer(s)
{
}


#pragma region ========== input game room ==========

void ServerInputGameRoom::Start()
{
	//reset value
	m_AppServer->m_OpenGameRoom = false;
	m_AppServer->m_JoinedClientNum = 0;
	m_AppServer->ClearMember();

	//test
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_SERVER_INPUTGAMEROOM, [this]()
		{
			ImGui::Begin("Server Game Room Setting");
			{
				//set player number
				ImGui::Text("target player number: ");
				ImGui::InputInt("num", &m_AppServer->m_TargetClientNum);

				//input port
				ImGui::InputInt("port", &m_AppServer->m_Port, 0, 65535);

				//input IP
				char ipBuf[LEN_IP];
				memcpy(ipBuf, m_AppServer->m_MyServer->m_IP.c_str(), LEN_IP);
				ImGui::InputText("RoomServerIP:", ipBuf, LEN_IP);
				m_AppServer->m_MyServer->m_IP = std::string(ipBuf);

				//confirm setting and start wait client
				if (ImGui::Button("Comfirm & Open Room"))
				{
					m_AppServer->m_OpenGameRoom = true;
				}
			}
			ImGui::End();
		});
}

void ServerInputGameRoom::Update()
{
	//test
	if (m_AppServer->m_OpenGameRoom)
	{
		//close panel
		DebugInfo::TestBlocks.erase(TESTBLOCKID_SERVER_INPUTGAMEROOM);

		//show chat panel
		MyNetManager::Instance()->SetChatPanel(true);

		//init socket and start asynselect
		//NetworkManager::Instance()->TryLaunchServer(m_OwnerServer->m_Port, true);
		//start
		m_AppServer->LaunchMyServer();

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
		m_AppServer->m_StartGame = false;
		m_WaitState = State::WAIT_CLIENT_REQUEST_OR_RESET;
	}

	//test
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_SERVER_WAITCLIENTINROOM,
		[this]()
		{
			ImGui::Begin("Server Wait Client Join");
			{
				//click to reset room setting
				if (m_WaitState == State::WAIT_CLIENT_REQUEST_OR_RESET)
				{
					if (ImGui::Button("ResetRoom"))
					{
						if (!m_AppServer->m_ResetGameRoom)
						{
							m_AppServer->m_ResetGameRoom = true;
							//return to imgui end
						}
					}
				}

				//show num
				ImGui::Text("players: %d / %d",
					m_AppServer->m_JoinedClientNum,
					m_AppServer->m_TargetClientNum);


				int readyCount = 0;
				for (auto idMem : m_AppServer->m_ClientMembers)
				{
					ClientMember& mem = *idMem.second;
					int id = idMem.first;
					ImGui::Text("%s [%s]",
						std::string("[" + std::to_string(id) + "]" + mem.Name).c_str(),
						mem.Ready ? std::string("ready").c_str() : std::string("wait").c_str());

					//show camp
					if (mem.Camp != CampType::NONE)
					{
						ImGui::Text(" [%s]", CampTypeName(mem.Camp).c_str());
					}

					//show pieces
					for (int i = 0; i < 4; i++)
					{
						if (mem.SelfPiece[i] != CharacterType::NONE)
						{
							ImGui::Text(" [%s]", CharacterTypeName(mem.SelfPiece[i]).c_str());
						}
					}


					if (mem.Ready)
					{
						readyCount++;
					}
				}


				//check can click start button
				if (m_WaitState == State::WAIT_CLIENT_REQUEST_OR_RESET &&
					readyCount == m_AppServer->m_TargetClientNum)
				{
					//click to set client camp and piece
					for (auto idMem : m_AppServer->m_ClientMembers)
					{
						ClientMember& mem = *idMem.second;
						int id = idMem.first;
						std::string buttonInfo =
							"set [" +
							std::to_string(mem.ID) +
							"]" +
							mem.Name;//+(mem.SetCampPiece ? "OK" : "...");

						//create set camp piece button
						if (ImGui::Button(buttonInfo.c_str()))
						{
							//jump to set client camp piece
							ServerSetClientCampPiece* setClient = new ServerSetClientCampPiece(m_AppServer, &mem);
							setClient->SetEndEvent("ServerWaitClientsJoin WAIT_CLIENT_REQUEST",
								[this]()
								{
									m_AppServer->BH_WaitClientsJoin->m_WaitState = State::WAIT_CLIENT_REQUEST_OR_RESET;
								});
							m_AppServer->JumpToBH(setClient);
							
							//set wait state
							m_WaitState = State::WAIT_SET_CLIENT_CAMPPIECE;
						}
					}

					
					//check client camp and piece is correct for mode
					bool canStart = false;
					{
						//check by camp need piece num
						int targetBadNum = Judgement::Instance()->m_Mode.GetBadNum();
						int targetGoodNum = Judgement::Instance()->m_Mode.GetGoodNum();
						int badCount=0, goodCount=0;
						for (auto idMem : m_AppServer->m_ClientMembers)
						{
							ClientMember& mem = *idMem.second;
							int id = idMem.first;
							for (int i = 0; i < 4; i++)
							{
								if (mem.SelfPiece[i] == CharacterType::NORMALBAD)
								{
									badCount++;
								}
								else if (mem.SelfPiece[i] == CharacterType::NORMALGOOD)
								{
									goodCount++;
								}
							}
						}

						//set canStart
						if (badCount == targetBadNum &&
							goodCount == targetGoodNum)
						{
							canStart = true;
						}
					}
					if (canStart)
					{
						//click to start game
						if (ImGui::Button("StartGame"))
						{
							if (!m_AppServer->m_StartGame)
							{
								m_AppServer->m_StartGame = true;
							}
						}
					}
				}
			}
			ImGui::End();
		});
}

void ServerWaitClientsJoin::Update()
{
	switch (m_WaitState)
	{
	case State::WAIT_CLIENT_REQUEST_OR_RESET:
	{
		PrintUpdateState("wait client request");

		//test reset room
		if (m_AppServer->m_ResetGameRoom)
		{
			//reset
			m_AppServer->m_ResetGameRoom = false;

			//set state
			m_WaitState = State::BACK_TO_INPUT_ROOM;
			break;
		}

		//test start game
		if (m_AppServer->m_JoinedClientNum == m_AppServer->m_TargetClientNum &&
			m_AppServer->m_StartGame)
		{
			//reset
			m_AppServer->m_StartGame = false;

			//reset client ready to false and add to judgement mode
			for (auto idMem : m_AppServer->m_ClientMembers)
			{
				//reset ready
				int id = idMem.first;
				ClientMember& mem = *idMem.second;
				m_AppServer->SetClientReady(id, false);

				//add player desc
				int pieceCount = 0;
				for (int i = 0; i < 4; i++)
				{
					if (mem.SelfPiece[i] != CharacterType::NONE)
					{
						pieceCount++;
					}
				}
				PlayerDesc playerDesc =
				{
					false,
					pieceCount,
					mem.Camp,
					mem.ID
				};
				Judgement::Instance()->m_Mode.AddPlayerDesc(playerDesc);
			}

			//set state
			m_WaitState = State::FINISH_WAIT;
		}
		break;
	}

	case State::BACK_TO_INPUT_ROOM:
	{
		PrintState("back to input room");

		//close wait join panel
		DebugInfo::TestBlocks.erase(TESTBLOCKID_SERVER_WAITCLIENTINROOM);

		//close chat panel
		DebugInfo::ClearNetMsg();
		MyNetManager::Instance()->SetChatPanel(false);

		//quit clients
		//GetNetSendFunc().Server_CommandDisconnect();
		//reset my server val and command client to disconnect
		MsgContent content = { MsgContent::CLIENT_DISCONNECT,
			0, nullptr };
		m_AppServer->m_MyServer->Reset(EncodeMsgContent(content));
		m_AppServer->m_MyServer->ShutDown();

		//next to input game room
		NextState();

		break;
	}

	case State::WAIT_SET_CLIENT_CAMPPIECE:
	{
		PrintUpdateState("wait set client camppiece");

		break;
	}

	case State::FINISH_WAIT:
	{
		PrintState("finish wait");

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
	AppServer* s,
	int memID,
	std::string memName,
	SOCKET* memSock,
	SOCKADDR_IN* memAddr):
	ServerBehaviour(s, true)
{
	m_CheckMember = new ClientMember;
	m_CheckMember->ID = memID;
	m_CheckMember->Name = memName;
	m_CheckMember->TCPSock = memSock;
	m_CheckMember->UDPAddr = memAddr;
	m_CheckMember->Ready = false;
}

void ServerCheckClientJoin::Start()
{
	if (m_AppServer->m_TargetClientNum > m_AppServer->m_JoinedClientNum)
		//can join
	{
		//add joined num
		m_AppServer->m_JoinedClientNum++;

		//register to client member
		m_AppServer->m_ClientMembers.emplace(m_CheckMember->ID, m_CheckMember);

		//agree request and send joined client infos to this client
		GetNetSendFunc().Server_AgreeJoin(m_CheckMember->ID, m_AppServer->m_TargetClientNum);
		GetNetSendFunc().Server_SendJoinedClientInfosToClient(m_CheckMember->ID, m_AppServer);

		//next to command other client members that new client joined 
		GetNetSendFunc().Server_CommandUpdateJoinedNum(m_CheckMember);

		//broadcast show join info after join
		std::string info =
			"- " +
			std::string("[" + std::to_string(m_CheckMember->ID) + "]") +
			m_CheckMember->Name +
			" join room";
		DebugInfo::PrintNetMsg(info.c_str());
		GetNetSendFunc().Server_BroadcastTestMsgToShow(info.c_str());
	}
	else
	{
		//command client continue to wait
		GetNetSendFunc().Server_DisagreeJoin(m_CheckMember->ID);

		delete m_CheckMember;

		//close tcp connect
		/*if (NetworkManager::Instance()->IsTCP)
		{
			NetworkManager::Instance()->CloseTCPSock(m_CheckMember->TCPSocketID);
		}*/
	}
}
#pragma endregion


#pragma region ========== set client camp piece ==========
ServerSetClientCampPiece::ServerSetClientCampPiece(
	AppServer* s, 
	ClientMember* mem):
	ServerBehaviour(s,true),
	m_SetClient(mem)
{
}

void ServerSetClientCampPiece::Start()
{
	//test
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_SERVER_SETCLIENTCAMPPIECE,
		[this]()
		{
			//set up piecec info panel pos
			ImGui::SetNextWindowPos(ImVec2(500, 100));
			ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
			std::string title =
				"Set Player[" +
				std::to_string(m_SetClient->ID) +
				"] camp and pieces";
			ImGui::Begin(title.c_str());
			{
				//set camp
				const char* campTypeItems[] = { "none", "bad man", "good man" };
				ImGui::Combo("camp", (int*)&m_SetClient->Camp, campTypeItems, IM_ARRAYSIZE(campTypeItems), 3);

				//set pieces
				if (m_SetClient->Camp == CampType::BAD)
				{

					for (int i = 0; i < 4; i++)
					{
						const char* characterTypeItems[] = { "none","normal bad" };
						std::string comboTitle = "piece" + std::to_string(i);
						ImGui::Combo(comboTitle.c_str(), (int*)&m_SetClient->SelfPiece[i], characterTypeItems, IM_ARRAYSIZE(characterTypeItems));
					}
				}
				else if (m_SetClient->Camp == CampType::GOOD)
				{
					for (int i = 0; i < 4; i++)
					{
						const char* characterTypeItems[] = { "none","normal good" };
						std::string comboTitle = "piece" + std::to_string(i);
						int charaType = (int)m_SetClient->SelfPiece[i];
						charaType = charaType != 0 ? charaType - 1 : 0;
						ImGui::Combo(comboTitle.c_str(), &charaType, characterTypeItems, IM_ARRAYSIZE(characterTypeItems));

						//cal correct character type
						m_SetClient->SelfPiece[i] = (CharacterType)(charaType != 0 ? charaType + 1 : 0);//offset = 1
					}
				}
				
				//confirm
				if (ImGui::Button("save"))
				{
					m_Finish = true;
				}
			}
			ImGui::End();
		});
}

void ServerSetClientCampPiece::Update()
{
	if (m_Finish)
	{
		//close set panel
		DebugInfo::CloseBlock(TESTBLOCKID_SERVER_SETCLIENTCAMPPIECE);

		//finish
		NextState();
	}
}
#pragma endregion

