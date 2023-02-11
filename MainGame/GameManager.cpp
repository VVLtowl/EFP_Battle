#include "main.h"
#include "Executor.h"

#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "Judgement.h"
#include "Board.h"
#include "Server.h"
#include "Client.h"

#include "GameManager.h"

#include "ServerBehaviour.h"
#include "ClientBehaviour.h"
#include "NetworkManager.h"

std::list<class Executor*> GameManager::m_Executors;

std::list<class Player*> GameManager::Players;
std::list<class Piece*> GameManager::Pieces;

void GameManager::SetUpGameScene()
{
	//test
	{
		//judgement init
		//todo

		//board init
		Board::Instance()->Init();

		//create players and pieces
		//test input player info
#if 0
		{
			//create bad man
			int playerCount = 0;
			for (int i = 0; i < PLAYER_BAD_START_NUM; i++)
			{
				playerCount++;

				Player* player = new Player();
				Piece* piece = new Piece(player);
				//set up piece
				{
					piece->Camp = CAMP_BAD;

					PieceObject* normalPawn = GameObjectManager::Create<PieceObject>();
					normalPawn->GetTransform()->SetPosition((float)i + 1, 1000, 0);
					piece->OwnPieceObject = normalPawn;
					
					Character* chara = new NormalBad(piece);
					piece->OwnCharacter = chara;
				}
				//set up player
				{
					player->OwnPieces.emplace_back(piece);
					player->Name = "BAD-" + std::to_string(playerCount);
				}

				//register player to game manager
				Players.emplace_back(player);
			}
			//create good man
			playerCount = 0;
			for (int i = 0; i < PLAYER_GOOD_START_NUM; i++)
			{
				playerCount++;

				Player* player = new Player();
				Piece* piece = new Piece(player);
				//set up piece
				{
					piece->Camp = CAMP_GOOD;

					PieceObject* normalPawn = GameObjectManager::Create<PieceObject>();
					normalPawn->GetTransform()->SetPosition((float)i + 1, 1000, 0);
					piece->OwnPieceObject = normalPawn;

					Character* chara = new NormalGood(piece);
					piece->OwnCharacter = chara;
				}
				//set up player
				{
					player->OwnPieces.emplace_back(piece);
					player->Name = "GOOD-" + std::to_string(playerCount);
				}

				//register player to game manager
				Players.emplace_back(player);
			}
		}
#endif
	}
}

void GameManager::CleanGameScene()
{
	//delete players
	for (auto player : Players)
	{
		delete player;
	}
	DebugInfo::Print("players size : " +
		std::to_string(Players.size()));
	Players.clear();

	//delete pieces
	for (auto piece : Pieces)
	{
		delete piece;
	}
	Pieces.clear();

	//judgement reset
	//Judgement::Instance()->Uninit();

	//board reset
	Board::Instance()->Uninit();
}


void GameManager::Init()
{
	//init singleton
	Judgement::Instance();
	Board::Instance();
}

void GameManager::Uninit()
{
	//clear new objects
	{
		for (auto player : Players)
		{
			delete player;
		}
		Players.clear();

		for (auto piece : Pieces)
		{
			delete piece;
		}
		Pieces.clear();
	}

	//check network close
	{
		NetworkManager::Instance()->Close();
	}
}

void GameManager::UpdateExecutors()
{
	for (auto excutor : m_Executors)
	{
		excutor->Execute();
	}
}