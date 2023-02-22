#include "main.h"

#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "Client.h"
#include "Server.h"
#include "Board.h"
#include "Judgement.h"

#include "BehaviourFactory.h"
#include "GameManager.h"
#include "JudgementBehaviour.h"
#include "ClientBehaviour.h"
#include "PieceBehaviour.h"
#include "NetworkManager.h"
#include "SceneManager.h"

#include "WorldToScreen.h"

#include "imgui/imgui.h"

void Judgement::Init()
{
	//set up judgement behaviour
	{
		BH_InputPlayMode = new JudgementInputPlayMode(this);
		BH_WaitPlayersFinish = new JudgementWaitPlayersFinish(this);
		BH_WaitPiecesFinishCmd = new JudgementWaitPiecesFinishCommand(this);
		BH_CommandShowPieces = new JudgementCommandShowPieces(this);
	}

	//test mode
	{
		//reset
		Mode.PlayerDescs.clear();

		PlayerDesc player1{ false,4,CampType::BAD};
		Mode.PlayerDescs.push_back(player1);

		PlayerDesc player2{ false,2,CampType::GOOD};
		Mode.PlayerDescs.push_back(player2);
	}
}

void Judgement::Uninit()
{
	//delete behaviour
	{
		while (m_Behaviours.empty() == false)
		{
			delete m_Behaviours.back();
		}
	}

	//test mode
	{
		Mode.PlayerDescs.clear();
	}
}

void Judgement::GameStart()
{
	//reset val
	{
		m_TurnCount = 0;
	}

	PrepareBoardAndPieces();

	SetPiecesInfoBlocks(true);

	StartWaitClientsPrepareBoard();
}

void Judgement::GameEnd()
{
	//close panel
	SetPiecesInfoBlocks(false);

	//uninit board
	Board::Instance()->Uninit();

	//clear players
	{
		for (auto idPlayer : GameManager::m_Players)
		{
			delete idPlayer.second;
		}
		GameManager::m_Players.clear();
	}

	//clear pieces
	{
		GameManager::m_Bads.clear();
		GameManager::m_Goods.clear();

		for (auto idPiece : GameManager::m_Pieces)
		{
			delete idPiece.second;
		}
		GameManager::m_Pieces.clear();
	}
}

#pragma region ========== judgement behaviour ==========

void Judgement::ShuffleAndSetPiecesToPlayers()
{
	//test
	//random set player desc to client by id order 
	if (0)
	{
		int num = Mode.GetPlayerNum();
		int count = 0;
		for (; num > 1; num--, count++)
		{
			int playerDescID = rand() % num;
			//todo: check can use move swap
			PlayerDesc temp = Mode.PlayerDescs[count];
			Mode.PlayerDescs[count] = Mode.PlayerDescs[playerDescID];
			Mode.PlayerDescs[playerDescID] = temp;
		}
	}

	//create players and pieces data in server
	int pieceCount = 0;
	int playerNum = Mode.GetPlayerNum();
	for (int playerCount = 0; playerCount < playerNum; playerCount++)
	{
		PlayerDesc& playerDesc = Mode.PlayerDescs[playerCount];
		Player* player = new Player();
		{
			//set camp
			player->m_Camp = playerDesc.PlayerCamp;

			//create pieces for player
			CampType pieceCamp = playerDesc.PlayerCamp;
			int pieceNum = playerDesc.PieceNum;
			for (int i = 0; i < pieceNum; i++)
			{
				//create piece and set
				Piece* piece = new Piece();
				{
					//set camp
					piece->m_Camp = pieceCamp;

					//set id
					piece->m_ID = pieceCount;

					//set owner player
					piece->m_OwnerPlayer = player;

					//test
					//set character
					if (pieceCamp == CampType::BAD)
					{
						piece->m_OwnCharacter = new NormalBad(piece);
						piece->m_CharaType = CharacterType::NORMALBAD;
					}
					else if (pieceCamp == CampType::GOOD)
					{
						piece->m_OwnCharacter = new NormalGood(piece);
						piece->m_CharaType = CharacterType::NORMALGOOD;
					}
				}

				//add to owner player
				player->m_SelfPieces.Add(piece);
				//add to factory
				GameManager::m_Pieces.emplace(piece->m_ID, piece);
				//add to camp list
				if (piece->m_Camp == CampType::BAD)
				{
					GameManager::m_Bads.emplace_back(piece);
				}
				else if (piece->m_Camp == CampType::GOOD)
				{
					GameManager::m_Goods.emplace_back(piece);
				}

				pieceCount++;
			}
		}
		GameManager::m_Players.emplace(playerCount, player);
	}

	//set id and client
	int playerCount = 0;
	for (auto client : Server::Instance()->m_ClientMembers)
	{
		Player* player = GameManager::m_Players[playerCount];
		playerCount++;
		{
			//pair player with client
			player->m_TargetClient = client.second;

			//set id
			player->m_ID = client.second->TCPSocketID;
		}
	}
}

void Judgement::SetPiecesInfoBlocks(bool show)
{
	//test 
	if (show)
	{
		//create pos hints
		for (auto idPiece : GameManager::m_Pieces)
		{
			int pieceID = idPiece.first;
			Piece* piece = idPiece.second;

			UIActMark* uiPos = new UIActMark(
				piece->m_PieceObject->
				GetUITransform(PieceObject::UITID::PAWN_UP_RIGHT)->GetTransform());
			uiPos->GetTransform()->SetPosition(0,20,0);
			m_PieceScreenPosHints.emplace(pieceID, uiPos);
		}

		//test create info panels
		DebugInfo::TestBlocks.emplace(TESTBLOCKID_JDUGEMENT_PIECEINFO, [this]()
			{
				for (auto idPiece : GameManager::m_Pieces)
				{
					Piece* piece = idPiece.second;
					GameObject* posHint = m_PieceScreenPosHints[idPiece.first];
					D3DXVECTOR3 pos = posHint->GetTransform()->GetWorldPosition();

					//set up piecec info panel pos
					ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y));
					ImGui::SetNextWindowSize(ImVec2(120, 150), ImGuiCond_FirstUseEver);

					//begin draw piece info panel
					std::string title = "Piece[" + std::to_string(piece->m_ID) + "] Info";
					ImGui::Begin(title.c_str());
					{
						//id
						ImGui::Text("id: %d",
							piece->m_ID);

						//owner client
						ImGui::Text("owner: \n client[%d] \n name[%s]",
							piece->m_OwnerPlayer->m_TargetClient->TCPSocketID,
							piece->m_OwnerPlayer->m_TargetClient->Name.c_str());

						//camp
						ImGui::Text("camp: %s",
							CampTypeName(piece->m_Camp).c_str());

						//actpoint
						ImGui::Text("actpoint: %d",
							piece->m_ActPoint);

						//hands
						ImGui::Text("hand:");
						for (auto hand : piece->m_Hands)
						{
							ImGui::Text("[%s]",
								Hand::HandName(hand).c_str());
						}
					}
					ImGui::End();
				}
			});
	}
	else
	{
		//close panels
		DebugInfo::CloseBlock(TESTBLOCKID_JDUGEMENT_PIECEINFO);

		//clear pos hints
		for (auto obj:m_PieceScreenPosHints)
		{
			obj.second->SetState(GameObject::DEAD);
		}
		m_PieceScreenPosHints.clear();
	}
	
}

void Judgement::CalculatePiecesActpoint()
{
	DebugInfo::Print("check pieces actpoint...");

	std::list<Piece*> goodPieces;
	std::list<Piece*> badPieces;

	//set up good pieces and bad pieces
	{
		for (auto idPiece : GameManager::m_Pieces)
		{
			Piece* piece = idPiece.second;
			if (piece->m_Camp == CampType::GOOD)
			{
				goodPieces.emplace_back(piece);
			}
			else if (piece->m_Camp == CampType::BAD)
			{
				badPieces.emplace_back(piece);
			}
		}
	}

	//battle
	for (auto idPiece : GameManager::m_Pieces)
	{
		Piece* self = idPiece.second;
		
		//check rivals
		std::list<Piece*>* rivals = nullptr;
		if (self->m_Camp == CampType::GOOD) { rivals = &badPieces; }
		else if (self->m_Camp == CampType::BAD) { rivals = &goodPieces; }

		//self's every hand battle with every rival's every hand 
		for (auto selfHand : self->m_Hands)
		{
			for (auto rival : *rivals)
			{
				for (auto rivalHand : rival->m_Hands)
				{
					if (RPS::Battle(selfHand, rivalHand) == ResultType::WIN)
					{
						//todo 
						//insteal by BH
						self->m_ActPoint++;
					}
				}
			}
		}
	}
}

void Judgement::PrepareBoardAndPieces()
{
	//prepare board and piece
	{
		//init board
		Board::Instance()->Init();

		//create piece obj and set pos
		for (auto idPiece : GameManager::m_Pieces)
		{
			Piece* piece = idPiece.second;

			PieceObject* normalPawn = GameObjectManager::Create<PieceObject>();
			piece->m_PieceObject = normalPawn;
			if (piece->m_Camp == CampType::BAD)
			{
				normalPawn->m_PawnImage->m_Polygon2D->Texture =
					DrawManager::Textures[TEXID_TEST_PIECE_NORMALBAD];
				Board::Instance()->SetPieceOnBadStartSquare(piece);
			}
			else if (piece->m_Camp == CampType::GOOD)
			{
				normalPawn->m_PawnImage->m_Polygon2D->Texture =
					DrawManager::Textures[TEXID_TEST_PIECE_NORMALGOOD];
				Board::Instance()->SetPieceOnGoodStartSquare(piece);
			}
		}
	}
}

#pragma endregion

#pragma region ========== start wait client player execute ==========
/*********************************************************
* @brief	game prepare
********************************************************/
void Judgement::StartWaitClientsCreatePlayer()
{
	//network command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitCreatePlayer");
	NetworkManager::Instance()->Judgement_CommandCreatePlayer();

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent("StartWaitClients SetTargetPieceNum", [this]()
		{
			DebugInfo::Print("- finish WaitCreatePlayer");

			//next
			StartWaitClientsSetTargetPieceNum();
		});

	//start wait players
	//tips: command notify may be faster than BH.Start()
	BH_WaitPlayersFinish->Reset();
	StartBH(BH_WaitPlayersFinish);
}

void Judgement::StartWaitClientsSetTargetPieceNum()
{
	//network command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitSetPieceNum");
	NetworkManager::Instance()->Judgement_CommandSetPieceNum();

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent("StartWaitClients CreatePiece", 
		[this]()
		{
			DebugInfo::Print("- finish WaitSetPieceNum");

			//next
			StartWaitClientsCreatePieces();
		});

	//start wait players
	//tips: command notify may be faster than BH.Start()
	BH_WaitPlayersFinish->Reset();
	StartBH(BH_WaitPlayersFinish);
}

void Judgement::StartWaitClientsCreatePieces()
{
	//network command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitCreatePieces");
	for (auto piece : GameManager::m_Pieces)
	{
		NetworkManager::Instance()->Judgement_CommandCreatePiece(piece.second);
	}

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent("StartWaitClients LoadGameScene", [this]()
		{
			DebugInfo::Print("- finish WaitCreatePieces");

			//next
			StartWaitClientsLoadGameScene();
		});

	//start wait pieces
	//tips: command notify may be faster than BH.Start()
	BH_WaitPlayersFinish->Reset();
	StartBH(BH_WaitPlayersFinish);
}

void Judgement::StartWaitClientsLoadGameScene()
{
	//network command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitLoadGameScene");
	NetworkManager::Instance()->Server_CommandLoadGameScene();
	
	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent("ChangeScene ToGameScene", [this]()
		{
			DebugInfo::Print("- finish WaitLoadGameScene");

			//next
			SceneManager::ChangeScene<ServerGameScene>();
			BH_WaitPlayersFinish->SetEndEvent("",[]() {; });//clear end event
		});

	//start wait pieces
	//tips: command notify may be faster than BH.Start()
	BH_WaitPlayersFinish->Reset();
	StartBH(BH_WaitPlayersFinish);
}


/*********************************************************
* @brief	board pepare
********************************************************/
void Judgement::StartWaitClientsPrepareBoard()
{
	//network server command
	//todo show square entry
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitPrepareBoard");

	//reset finish count
	BH_WaitPlayersFinish->Reset();

	//start command clients show pieces entry
	BH_CommandShowPieces->Reset(10, GameManager::m_Pieces.size(),
		[this]() //command event
		{
			//send command
			Piece* piece = (*BH_CommandShowPieces->PieceIterator).second;
			NetworkManager::Instance()->Judgement_CommandPieceShowEntry(piece);
		});
	BH_CommandShowPieces->SetEndEvent("StartWaitClients ShowPiecesEntry",
		[this]() //end event
		{
			StartWaitClientsShowPiecesEntry();
		});
	StartBH(BH_CommandShowPieces);

	DebugInfo::Print("- finish WaitPrepareBoard");
}


/*********************************************************
* @brief	make clients show step
********************************************************/
void Judgement::StartWaitClientsShowStepGameStart()
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClientsShowStep");
	NetworkManager::Instance()->
		Judgement_CommandShowStep((int)ShowStepType::GAME_START);

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent("StartWaitClients ShowStepTurnStart",
		[this]()
		{
			DebugInfo::Print("- finish WaitClientsShowStep");

			//next
			StartWaitClientsShowStepTurnStart();
		});

	//start wait pieces
	//tips: command notify may be faster than BH.Start()
	BH_WaitPlayersFinish->Reset();
	StartBH(BH_WaitPlayersFinish);
}

void Judgement::StartWaitClientsShowStepTurnStart()
{
	//update turn count
	m_TurnCount++;

	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClientsShowTurnStart");
	NetworkManager::Instance()->
		Judgement_CommandShowStep((int)ShowStepType::TURN_START);

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent("StartWaitClients ShowStepRPSStart", 
		[this]()
		{
			DebugInfo::Print("- finish WaitClientsShowTurnStart");

			//next
			StartWaitClientsShowStepRPSStart();
		});

	//start wait pieces
	//tips: command notify may be faster than BH.Start()
	BH_WaitPlayersFinish->Reset();
	StartBH(BH_WaitPlayersFinish);
}

void Judgement::StartWaitClientsShowStepRPSStart()
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClientsShowRPSStart");
	NetworkManager::Instance()->
		Judgement_CommandShowStep((int)ShowStepType::RPS_START);

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent("StartWaitClients InputRPS", 
		[this]()
		{
			DebugInfo::Print("- finish WaitClientsShowRPSStart");

			//next
			StartWaitClientsInputRPS();
		});

	//start wait pieces
	//tips: command notify may be faster than BH.Start()
	BH_WaitPlayersFinish->Reset();
	StartBH(BH_WaitPlayersFinish);
}

void Judgement::StartWaitClientsShowStepActStart()
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClientsShowActStart");
	NetworkManager::Instance()->
		Judgement_CommandShowStep((int)ShowStepType::ACT_START);

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent("StartIteratePieces InputAct", 
		[this]()
		{
			DebugInfo::Print("- finish WaitClientsShowActStart");

			//next
			StartIteratePiecesInputAct();
		});

	//start wait pieces
	//tips: command notify may be faster than BH.Start()
	BH_WaitPlayersFinish->Reset();
	StartBH(BH_WaitPlayersFinish);
}


/*********************************************************
* @brief	manage pieces RPS
********************************************************/
void Judgement::StartWaitClientsInputRPS()
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClientsInputRPS");
	NetworkManager::Instance()->Judgement_CommandInputRPS();

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent("StartTimerCommand ShowPiecesHandResult", 
		[this]()
		{
			DebugInfo::Print("- finish WaitClientsInputRPS");

			//next
			StartCommandShowPiecesHand();
		});

	//start wait pieces
	//tips: command notify may be faster than BH.Start()
	BH_WaitPlayersFinish->Reset();
	StartBH(BH_WaitPlayersFinish);
}


void Judgement::StartCommandShowPiecesHand()
{
	//reset finish count
	BH_WaitPlayersFinish->Reset();

	//start wait bh after finish sending all clients' command send 
	BH_CommandShowPieces->Reset(5, GameManager::m_Pieces.size(),
		[this]() //command event
		{
			//send command
			Piece* piece = (*BH_CommandShowPieces->PieceIterator).second;
			NetworkManager::Instance()->Judgement_CommandShowPieceHand(piece);
		});
	BH_CommandShowPieces->SetEndEvent("StartWaitClients ShowPiecesHandResult",
		[this]() //end event
		{
			StartWaitClientsShowPiecesHandResult();
		});
	StartBH(BH_CommandShowPieces);
}

void Judgement::StartIteratePiecesShowCheckActpoint()
{
	//tips: sync by client check actpoint
	//check and save actpoint data in server judgement
	//CalculatePiecesActpoint();

	//reset piece iterator
	BH_WaitPiecesFinishCmd->Reset([this]()//command event
		{
			Piece* piece = (*BH_WaitPiecesFinishCmd->PieceIterator);

			if (piece->m_State == Piece::State::NORMAL)
				//state normal and state caught piece need to check
			{
				NetworkManager::Instance()->Judgement_CommandPieceShowCheckActpoint(piece);

				//jump to wait client finish cmd
				BH_WaitPlayersFinish->Reset();
				BH_WaitPlayersFinish->SetEndEvent("StartWaitClients ShowPiecesEntry", 
					[this]()
					{
						BH_WaitPiecesFinishCmd->WaitState =
							JudgementWaitPiecesFinishCommand::State::PIECE_FINISH_COMMAND;
					});

				//set waitPieceFinishCmd next state
				BH_WaitPiecesFinishCmd->WaitState =
					JudgementWaitPiecesFinishCommand::State::WAIT_PIECE_FINISH_COMMAND;
				JumpToBH(BH_WaitPlayersFinish);
			}
			else
				//state escape  needn't check actpoint
			{
				BH_WaitPiecesFinishCmd->WaitState =
					JudgementWaitPiecesFinishCommand::State::PIECE_FINISH_COMMAND;
			}
		});
	BH_WaitPiecesFinishCmd->SetEndEvent("StartWaitClients ShowStepActStart", 
		[this]()
		{
			StartWaitClientsShowStepActStart();
		});
	StartBH(BH_WaitPiecesFinishCmd);
}

void Judgement::StartIteratePiecesInputAct()
{
	//reset piece iterator
	BH_WaitPiecesFinishCmd->Reset([this]()//command event
		{
			Piece* piece = (*BH_WaitPiecesFinishCmd->PieceIterator);

			if (piece->m_State == Piece::State::NORMAL)
				//only state normal piece need to act
			{
				NetworkManager::Instance()->Judgement_CommandPieceInputAct(piece);
				BH_WaitPiecesFinishCmd->WaitState =
					JudgementWaitPiecesFinishCommand::State::WAIT_PIECE_FINISH_COMMAND;
			}
			else
				//state escape and state caught don't act
			{
				BH_WaitPiecesFinishCmd->WaitState =
					JudgementWaitPiecesFinishCommand::State::PIECE_FINISH_COMMAND;
			}
		});
	BH_WaitPiecesFinishCmd->SetEndEvent("StartCommandPiecesClearActpt",
		[this]()
		{
			StartCommandPiecesClearActpoint();
			//StartWaitClientsShowStepTurnStart();
		});
	StartBH(BH_WaitPiecesFinishCmd);
}

void Judgement::StartCommandPiecesClearActpoint()
{
	//need clear judgement server hand and actpoint
	//command pieces clear actpoint
	NetworkManager::Instance()->Judgement_CommandPiecesClearHandAndActpoint();

	//set up waitBH
	BH_WaitPlayersFinish->Reset();
	BH_WaitPlayersFinish->SetEndEvent("StartWaitClients ShowStepTurnStart", 
		[this]()
		{
			//next
			StartWaitClientsShowStepTurnStart();
		});
	StartBH(BH_WaitPlayersFinish);
}

void Judgement::StartIteratePiecesClearActpoint()
{
	//reset piece iterator
	BH_WaitPiecesFinishCmd->Reset(
		[this]()//command event
		{
			Piece* piece = (*BH_WaitPiecesFinishCmd->PieceIterator);

			if (piece->m_State == Piece::State::NORMAL)
				//only state normal piece need to act
			{
				NetworkManager::Instance()->Judgement_CommandPieceInputAct(piece);
				BH_WaitPiecesFinishCmd->WaitState =
					JudgementWaitPiecesFinishCommand::State::WAIT_PIECE_FINISH_COMMAND;
			}
			else
				//state escape and state caught don't act
			{
				BH_WaitPiecesFinishCmd->WaitState =
					JudgementWaitPiecesFinishCommand::State::PIECE_FINISH_COMMAND;
			}
		});
	BH_WaitPiecesFinishCmd->SetEndEvent("StartIteratePiecesClearHand",
		[this]()//end event
		{
			StartIteratePiecesClearHand();
		});
	StartBH(BH_WaitPiecesFinishCmd);
}

void Judgement::StartIteratePiecesClearHand()
{
	//reset piece iterator
	BH_WaitPiecesFinishCmd->Reset(
		[this]()//command event
		{
			Piece* piece = (*BH_WaitPiecesFinishCmd->PieceIterator);

			if (piece->m_State == Piece::State::NORMAL)
				//only state normal piece need to act
			{
				NetworkManager::Instance()->Judgement_CommandPieceInputAct(piece);
				BH_WaitPiecesFinishCmd->WaitState =
					JudgementWaitPiecesFinishCommand::State::WAIT_PIECE_FINISH_COMMAND;
			}
			else
				//state escape and state caught don't act
			{
				BH_WaitPiecesFinishCmd->WaitState =
					JudgementWaitPiecesFinishCommand::State::PIECE_FINISH_COMMAND;
			}
		});
	BH_WaitPiecesFinishCmd->SetEndEvent("StartWaitClientsShowStepTurnStart",
		[this]()//end event
		{
			StartWaitClientsShowStepTurnStart();
		});
	StartBH(BH_WaitPiecesFinishCmd);
}


/*********************************************************
* @brief	manage pieces act
********************************************************/
void Judgement::StartWaitClientsPieceFinishAct(int pieceID)
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClientsPieceFinishAct");
	NetworkManager::Instance()->
		Judgement_CommandPieceFinishAct(pieceID);

	//tips: clear judgement server hand and actpoint must be at last
	//clear judgement pieces' hand and actpoint
	//Piece* piece = m_Pieces[pieceID];
	//piece->m_Hands.clear();
	//piece->m_ActPoint = 0;

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent("PieceFinishCmd InputAct", 
		[this]()
		{
			DebugInfo::Print("- finish WaitClientsPieceFinishAct");

			//next
			BH_WaitPiecesFinishCmd->WaitState =
				JudgementWaitPiecesFinishCommand::State::PIECE_FINISH_COMMAND;
		});
	BH_WaitPlayersFinish->Reset();
	JumpToBH(BH_WaitPlayersFinish);
}

void Judgement::CheckPieceMove(int pieceID, int squareID)
{
	DebugInfo::Print("check piece move...");

	//change piece and square data in judgement at first
	Piece* piece = GameManager::m_Pieces[pieceID];
	Square* oldSqr = piece->m_FootOnSquare;
	Square* newSqr = Board::Instance()->Squares[squareID];

	//check endEvent of move
	std::string endInfo = "";
	std::function<void()> moveEndEvent = []() {; };
	switch (newSqr->CheckEnter(piece))
	{
	case Square::EnterResult::CATCH_BAD:
	{
		endInfo = "CheckPieceCaught";
		Piece* caughtPiece = newSqr->m_Piece;
		moveEndEvent = [this,piece,caughtPiece]()
		{
			CheckPieceCaught(piece->m_ID,caughtPiece->m_ID);
		};
		break;
	}

	case Square::EnterResult::ESCAPE:
	{
		endInfo = "CheckPieceEscape";
		Piece* escapePiece = piece;
		moveEndEvent = [this, escapePiece]()
		{
			//StartWaitClientsPieceCaught(escapePiece->ID);
			CheckPieceEscape(escapePiece->m_ID);
		};

		break;
	}

	case Square::EnterResult::NORMAL_ENTER:
	default:
	{
		endInfo = "Judgement_CommandPieceContinueMoveOrInputAct";
		moveEndEvent = [this,piece]()
		{
			if (piece->m_ActPoint > 0)
			{
				NetworkManager::Instance()->
					Judgement_CommandPieceContinueMove(piece->m_OwnerPlayer->m_TargetClient, piece->m_ID);
			}
			else
			{
				NetworkManager::Instance()->
					Judgement_CommandPieceInputAct(piece);
			}
		};

		break;
	}
	}

	//update square data and piece data in server
	{
		//piece event: out square
		piece->m_FootOnSquare = nullptr;

		//square event: piece out 
		oldSqr->m_Piece = nullptr;

		//piece event: in square
		piece->m_ActPoint--;
		piece->m_FootOnSquare = newSqr;
		piece->m_PieceObject->GetTransform()->
			SetPosition(newSqr->m_SquareObject->GetTransform()->GetWorldPosition());

		//square event: piece in
		newSqr->m_Piece = piece;
	}

	//start clients piece move
	StartWaitClientsPieceMove(pieceID, squareID, endInfo, moveEndEvent);
}
void Judgement::StartWaitClientsPieceMove(int pieceID,int squareID, std::string endEventInfo,std::function<void()> endEvent)
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClientsPieceMove");
	NetworkManager::Instance()->
		Judgement_CommandPieceMove(pieceID,squareID);

	//set what to do after wait
	BH_WaitPlayersFinish->Reset();
	BH_WaitPlayersFinish->SetEndEvent("StartWaitClients ShowPiecesEntry", 
		[endEvent]()
		{
			DebugInfo::Print("- finish WaitClientsPieceMove");
			endEvent();
		});

	//tips: main wait is iterate pieces cmd
	//BH_WaitPiecesFinishCmd->JumpTo(BH_WaitPlayersFinish);
	JumpToBH(BH_WaitPlayersFinish);
}

void Judgement::CheckPieceEscape(int pieceID)
{
	DebugInfo::Print("check piece escape...");

	//change piece and square data in judgement at first
	Piece* piece = GameManager::m_Pieces[pieceID];
	Square* oldSqr = piece->m_FootOnSquare;
	Square* escapeSqr = Board::Instance()->GetPrisonRoomSquare();

	//check endEvent of escape
	std::string endInfo = "StartWaitClientsGameOver BadWin";
	std::function<void()> escapeEndEvent = [this]()
	{
		//test
		//when one bad man escape, bad camp win
		StartWaitClientsGameOver((int)ClientShowGameOver::ResultType::BAD_WIN);
	};

	//update square data and piece data in server
	{
		//piece event: out square
		piece->m_FootOnSquare = nullptr;

		//square event: piece out 
		oldSqr->m_Piece = nullptr;

		//piece event: in square
		piece->m_FootOnSquare = escapeSqr;
		piece->m_PieceObject->GetTransform()->
			SetPosition(escapeSqr->m_SquareObject->GetTransform()->GetWorldPosition());

		//square event: piece in
		escapeSqr->m_Piece = piece;
	}

	//start clients piece move
	int squareID = escapeSqr->m_ID;
	StartWaitClientsPieceEscape(pieceID, squareID, endInfo, escapeEndEvent);
}
void Judgement::StartWaitClientsPieceEscape(int pieceID,int goalSquareID, std::string endEventInfo, std::function<void()> endEvent)
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClients PieceEscape");
	NetworkManager::Instance()->
		Judgement_CommandPieceEscape(pieceID, goalSquareID);

	//set what to do after wait
	BH_WaitPlayersFinish->Reset();
	BH_WaitPlayersFinish->SetEndEvent(endEventInfo,
		[endEvent]()
		{
			DebugInfo::Print("- finish WaitClients PieceEscape");
			endEvent();
		});

	//tips: main wait is iterate pieces cmd
	//BH_WaitPiecesFinishCmd->JumpTo(BH_WaitPlayersFinish);
	JumpToBH(BH_WaitPlayersFinish);
}

void Judgement::CheckPieceCaught(int movePieceID, int caughtPieceID)
{
	DebugInfo::Print("check piece caught...");

	//set target piece caught
	Piece* movePiece = GameManager::m_Pieces[movePieceID];
	Piece* caughtPiece = GameManager::m_Pieces[caughtPieceID];
	caughtPiece->m_State = Piece::State::CAUGHT;

	//check end event of caught
	std::string endInfo = "";
	std::function<void()> caughtEndEvent = []() {; };
	{
		//test
		//todo: create check values
		//calculate caught num 
		bool badsAreAllCaught = true;
		for (auto bad : GameManager::m_Bads)
		{
			if (bad->m_State == Piece::State::NORMAL)
			{
				badsAreAllCaught = false;
			}
		}
		//when all bads are caught, good camp player win
		if (badsAreAllCaught)
		{
			endInfo = "StartWaitClientsGameOver GoodWin";
			caughtEndEvent = [this]()
			{
				//game over event: good camp player win 
				StartWaitClientsGameOver((int)ClientShowGameOver::ResultType::GOOD_WIN);
			};
		}
		else
		{
			endInfo = "Judgement_CommandPieceContinueMoveOrInputAct";
			caughtEndEvent = [this, movePiece]()
			{
				if (movePiece->m_ActPoint > 0)
				{
					NetworkManager::Instance()->
						Judgement_CommandPieceContinueMove(movePiece->m_OwnerPlayer->m_TargetClient, movePiece->m_ID);
				}
				else
				{
					NetworkManager::Instance()->
						Judgement_CommandPieceInputAct(movePiece);
				}
			};
		}
	}

	//update piece data and prison room square data
	Square* prisonRoom = Board::Instance()->GetPrisonRoomSquare();
	prisonRoom->m_Piece = caughtPiece;
	caughtPiece->m_FootOnSquare = prisonRoom;
	caughtPiece->m_PieceObject->GetTransform()->SetPosition(
		prisonRoom->m_SquareObject->GetTransform()->GetWorldPosition());

	//start clients piece caught
	int prisonRoomSquareID = prisonRoom->m_ID;
	StartWaitClientsPieceCaught(movePieceID, caughtPieceID, prisonRoomSquareID, endInfo, caughtEndEvent);
}
void Judgement::StartWaitClientsPieceCaught(int movePieceID, int caughtPieceID, int prisonRoomSquareID, std::string endEventInfo, std::function<void()> endEvent)
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClients PieceCaught");
	NetworkManager::Instance()->
		Judgement_CommandPieceCaught(movePieceID, caughtPieceID, prisonRoomSquareID);

	//set what to do after wait
	BH_WaitPlayersFinish->Reset();
	BH_WaitPlayersFinish->SetEndEvent(endEventInfo,
		[endEvent]()
		{
			DebugInfo::Print("- finish WaitClients PieceCaught");
			endEvent();
		});

	//tips: main wait is iterate pieces cmd
	//BH_WaitPiecesFinishCmd->JumpTo(BH_WaitPlayersFinish);
	JumpToBH(BH_WaitPlayersFinish);
}



/*********************************************************
* @brief	make client piece show 
********************************************************/
void Judgement::StartWaitClientsShowPiecesEntry()
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClients ShowPiecesEntry");

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent("StartWaitClients ShowStepGameStart",
		[this]()
		{
			DebugInfo::Print("- finish WaitClientsShowPiecesEntry");

			//next
			StartWaitClientsShowStepGameStart();
			//StartWaitClientsInputRPS();
		});
	//start wait pieces
	//tips: command notify may be faster than BH.Start()
	BH_WaitPlayersFinish->Reset();
	StartBH(BH_WaitPlayersFinish);
}

void Judgement::StartWaitClientsShowPiecesHandResult()
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClients ShowPiecesHandResult");

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent("StartIteratePieces ShowCheckActpt", 
		[this]()
		{
			DebugInfo::Print("- finish WaitClientsShowPiecesHandResult");

			//next
			StartIteratePiecesShowCheckActpoint();
			//BH_WaitPlayersFinish->SetEndEvent([]() {; });//clear end event
		});

	//start wait pieces
	//tips: command notify may be faster than BH.Start()
	StartBH(BH_WaitPlayersFinish);
}


/*********************************************************
* @brief	manage game over
********************************************************/
void Judgement::StartWaitClientsGameOver(int result)
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClients GameOver");
	NetworkManager::Instance()->Judgement_CommandShowGameOver(result);


	//set what to do after wait
	//tips: stop wait piece finish cmd at first
	BH_WaitPlayersFinish->Reset();
	BH_WaitPlayersFinish->SetEndEvent("ChangeScene ToServerSettingScene",
		[this]()
		{
			DebugInfo::Print("- finish WaitClients GameOver");

			//next
			SceneManager::ChangeScene<ServerSettingScene>();
		});
	//start wait pieces
	//tips: command notify may be faster than BH.Start()
	StartBH(BH_WaitPlayersFinish);
}


#pragma endregion


