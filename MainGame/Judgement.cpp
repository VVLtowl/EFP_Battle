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
#include "PieceBehaviour.h"
#include "NetworkManager.h"

#include "SceneManager.h"

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


	m_MainLoopBehaviours.clear();
	m_SpecialBehaviours.clear();
}

void Judgement::GameStart()
{
	PrepareBoard();
	StartWaitClientsPrepareBoard();
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
	int playerCount = 0;
	int pieceCount = 0;
	for (auto client : Server::Instance()->ClientMembers)
	{
		//create player
		PlayerDesc& playerDesc = Mode.PlayerDescs[playerCount];
		playerCount++;
		playerDesc.ClientID = client.second->ID;

		Player* player = new Player();
		{
			//pair player with client
			player->TargetClient = client.second;

			//set id
			player->ID = playerDesc.ClientID;

			//set camp
			player->Camp = playerDesc.PlayerCamp;

			//create pieces for player
			CampType pieceCamp = playerDesc.PlayerCamp;
			int pieceNum = playerDesc.PieceNum;
			for (int i = 0; i < pieceNum; i++)
			{
				//create piece and set
				Piece* piece = new Piece();
				{
					//set camp
					piece->Camp = pieceCamp;

					//set id
					piece->ID = pieceCount;

					//set owner player
					piece->OwnerPlayer = player;

					//test
					//set character
					if (pieceCamp == CampType::BAD)
					{
						piece->OwnCharacter = new NormalBad(piece);
						piece->CharaType = CharacterType::NORMALBAD;
					}
					else if (pieceCamp == CampType::GOOD)
					{
						piece->OwnCharacter = new NormalGood(piece);
						piece->CharaType = CharacterType::NORMALGOOD;
					}
				}

				//add to owner player
				player->SelfPieces.Add(piece);
				//add to factory
				Pieces.emplace(piece->ID, piece);
				//add to camp list
				if (piece->Camp == CampType::BAD)
				{
					Bads.emplace_back(piece);
				}
				else if (piece->Camp == CampType::GOOD)
				{
					Goods.emplace_back(piece);
				}

				pieceCount++;
			}
		}
		Players.emplace(player->ID,player);
	}
}

void Judgement::PrepareBoard()
{
	//init board
	Board::Instance()->Init();

	//create piece obj and set pos
	for (auto idPiece : Pieces)
	{
		Piece* piece = idPiece.second;

		PieceObject* normalPawn = GameObjectManager::Create<PieceObject>();
		piece->m_PieceObject = normalPawn;
		if (piece->Camp == CampType::BAD)
		{
			Board::Instance()->SetPieceOnBadStartSquare(piece);
		}
		else if(piece->Camp==CampType::GOOD)
		{
			Board::Instance()->SetPieceOnGoodStartSquare(piece);
		}
	}
}

void Judgement::CheckPiecesActpoint()
{
	DebugInfo::Print("check pieces actpoint...");

	std::list<Piece*> goodPieces;
	std::list<Piece*> badPieces;

	//set up good pieces and bad pieces
	{
		for (auto idPiece : Pieces)
		{
			Piece* piece = idPiece.second;
			if (piece->Camp == CampType::GOOD)
			{
				goodPieces.emplace_back(piece);
			}
			else if (piece->Camp == CampType::BAD)
			{
				badPieces.emplace_back(piece);
			}
		}
	}

	//battle
	for (auto idPiece : Pieces)
	{
		Piece* self = idPiece.second;
		
		//check rivals
		std::list<Piece*>* rivals = nullptr;
		if (self->Camp == CampType::GOOD) { rivals = &badPieces; }
		else if (self->Camp == CampType::BAD) { rivals = &goodPieces; }

		//self's every hand battle with every rival's every hand 
		for (auto selfHand : self->Hands)
		{
			for (auto rival : *rivals)
			{
				for (auto rivalHand : rival->Hands)
				{
					if (RPS::Battle(selfHand, rivalHand) == ResultType::WIN)
					{
						//todo 
						//insteal by BH
						self->ActPoint++;
					}
				}
			}
		}
	}
}

#pragma endregion

#pragma region ========== start wait client player execute ==========

void Judgement::StartWaitClientsCreatePlayer()
{
	//network command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitCreatePlayer");
	NetworkManager::Instance()->Judgement_CommandCreatePlayer();

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent([this]()
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
	BH_WaitPlayersFinish->SetEndEvent([this]()
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
	for (auto piece : Pieces)
	{
		NetworkManager::Instance()->Judgement_CommandCreatePiece(piece.second);
	}

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent([this]()
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
	BH_WaitPlayersFinish->SetEndEvent([this]()
		{
			DebugInfo::Print("- finish WaitLoadGameScene");

			//next
			SceneManager::ChangeScene<ServerGameScene>();
			BH_WaitPlayersFinish->SetEndEvent([]() {; });//clear end event
		});

	//start wait pieces
	//tips: command notify may be faster than BH.Start()
	BH_WaitPlayersFinish->Reset();
	StartBH(BH_WaitPlayersFinish);
}



void Judgement::StartWaitClientsShowStepGameStart()
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClientsShowStep");
	NetworkManager::Instance()->
		Judgement_CommandShowStep((int)ShowStepType::GAME_START);

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent([this]()
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
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClientsShowTurnStart");
	NetworkManager::Instance()->
		Judgement_CommandShowStep((int)ShowStepType::TURN_START);

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent([this]()
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
	BH_WaitPlayersFinish->SetEndEvent([this]()
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
	BH_WaitPlayersFinish->SetEndEvent([this]()
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



void Judgement::StartWaitClientsInputRPS()
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClientsInputRPS");
	NetworkManager::Instance()->Judgement_CommandInputRPS();

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent([this]()
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



void Judgement::StartWaitClientsPrepareBoard()
{
	//network server command
	//todo show square entry
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitPrepareBoard");

	//reset finish count
	BH_WaitPlayersFinish->Reset();

	//start command clients show pieces entry
	BH_CommandShowPieces->Reset(10, Pieces.size(),
		[this]() //command event
		{
			//send command
			Piece* piece = (*BH_CommandShowPieces->PieceIterator).second;
			NetworkManager::Instance()->Judgement_CommandPieceShowEntry(piece);
		});
	BH_CommandShowPieces->SetEndEvent(
		[this]() //end event
		{
			StartWaitClientsShowPiecesEntry();
		});
	StartBH(BH_CommandShowPieces);

	DebugInfo::Print("- finish WaitPrepareBoard");
}

void Judgement::StartCommandShowPiecesHand()
{
	//reset finish count
	BH_WaitPlayersFinish->Reset();

	//start wait bh after finish sending all clients' command send 
	BH_CommandShowPieces->Reset(5, Pieces.size(),
		[this]() //command event
		{
			//send command
			Piece* piece = (*BH_CommandShowPieces->PieceIterator).second;
			NetworkManager::Instance()->Judgement_CommandShowPieceHand(piece);
		});
	BH_CommandShowPieces->SetEndEvent(
		[this]() //end event
		{
			StartWaitClientsShowPiecesHandResult();
		});
	StartBH(BH_CommandShowPieces);
}

void Judgement::StartIteratePiecesShowCheckActpoint()
{
	//check and save actpoint data in server judgement
	CheckPiecesActpoint();

	//reset piece iterator
	BH_WaitPiecesFinishCmd->Reset([this]()//command event
		{
			Piece* piece = (*BH_WaitPiecesFinishCmd->PieceIterator);
			NetworkManager::Instance()->Judgement_CommandPieceShowCheckActpoint(piece);
			
			//jump to wait client finish cmd
			BH_WaitPlayersFinish->Reset();
			BH_WaitPlayersFinish->SetEndEvent([this]()
				{
					BH_WaitPiecesFinishCmd->WaitState =
						JudgementWaitPiecesFinishCommand::State::PIECE_FINISH_COMMAND;
				});
			JumpToBH(BH_WaitPlayersFinish);
		});
	BH_WaitPiecesFinishCmd->SetEndEvent([this]()
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
			NetworkManager::Instance()->Judgement_CommandPieceInputAct(piece);
		});
	BH_WaitPiecesFinishCmd->SetEndEvent([this]()
		{
			StartCommandPiecesClearActpoint();
			//StartWaitClientsShowStepTurnStart();
		});
	StartBH(BH_WaitPiecesFinishCmd);
}

void Judgement::StartCommandPiecesClearActpoint()
{
	//command pieces clear actpoint
	NetworkManager::Instance()->Judgement_CommandPiecesClearHandAndActpoint();

	//set up waitBH
	BH_WaitPlayersFinish->Reset();
	BH_WaitPlayersFinish->SetEndEvent([this]()
		{
			//next
			StartWaitClientsShowStepTurnStart();
		});
	StartBH(BH_WaitPlayersFinish);
}


void Judgement::StartWaitClientsPieceFinishAct(int pieceID)
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClientsPieceFinishAct");
	NetworkManager::Instance()->
		Judgement_CommandPieceFinishAct(pieceID);

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent([this]()
		{
			DebugInfo::Print("- finish WaitClientsPieceFinishAct");

			//next
			BH_WaitPiecesFinishCmd->WaitState =
				JudgementWaitPiecesFinishCommand::State::PIECE_FINISH_COMMAND;
		});
	BH_WaitPlayersFinish->Reset();
	JumpToBH(BH_WaitPlayersFinish);
}

void Judgement::StartCheckPieceMove(int pieceID)
{

}
void Judgement::StartWaitClientsPieceMove(int pieceID)
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClientsPieceMove");
	NetworkManager::Instance()->
		Judgement_CommandPieceFinishAct(pieceID);

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent([this]()
		{
			DebugInfo::Print("- finish WaitClientsPieceMove");

			//next
			//todo
		});
	BH_WaitPlayersFinish->Reset();
	JumpToBH(BH_WaitPlayersFinish);
}

void Judgement::StartCheckPieceEscape(int pieceID)
{
}
void Judgement::StartWaitClientsPieceEscape(int pieceID)
{

}

void Judgement::StartCheckPieceCaught(int pieceID)
{
}
void Judgement::StartWaitClientsPieceCaught(int pieceID)
{

}




void Judgement::StartWaitClientsShowPiecesEntry()
{
	//network server command
	DebugInfo::Print("");
	DebugInfo::Print("- start WaitClientsShowPiecesEntry");

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent([this]()
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
	DebugInfo::Print("- start WaitClientsShowPiecesHandResult");

	//set what to do after wait
	BH_WaitPlayersFinish->SetEndEvent([this]()
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


#pragma endregion


