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

#include "imgui/imgui.h"

std::list<class Executor*> GameManager::m_Executors;

std::unordered_map<int,class Player*> GameManager::m_Players;
std::unordered_map<int,class Piece*> GameManager::m_Pieces;
std::list<class Piece*> GameManager::m_Goods;
std::list<class Piece*> GameManager::m_Bads;


/*
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
*/

void GameManager::Init()
{
	//init singleton
	//Judgement::Instance();
	//Board::Instance();
	m_Executors.clear();

	//test
	//show executor info block
	//test
	DebugInfo::TestBlocks.emplace(TESTBLOCKID_EXECUTORSINFO, []()
		{
			ImGui::Begin("Executor Info");

			for (auto exec : m_Executors)
			{
				std::string executorName = exec->Name()+": ";
				std::string currentBHName = "";
				std::string currentBHState = "";
				std::string nextBHName = "";
				std::string endEventInfo = "";
				if (exec->GetBH())
				{
					//set currentBHName
					currentBHName=exec->GetBH()->Name();

					//set currentBHState
					currentBHState = "[" + exec->GetBH()->GetStateName() + "] ";

					//set nextBHName
					if (exec->GetBH()->GetNext())
					{
						nextBHName = " (next: [" + exec->GetBH()->GetNext()->Name() + "])";
					}

					//set endEventInfo
					if (exec->GetBH()->HasEndEvent())
					{
						endEventInfo = " (end event: [" + exec->GetBH()->GetEndEventInfo() + "])";
					}
				}
				
				std::string rowInfo =
					executorName +
					currentBHState + currentBHName +
					nextBHName +
					endEventInfo;

				ImGui::Text("%s", rowInfo.c_str());
			}

			ImGui::End();
		});
}

void GameManager::Uninit()
{
	//close info panel before clear objs
	DebugInfo::CloseBlock(TESTBLOCKID_EXECUTORSINFO);

	//clear
	Clear();

	/*
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
	*/
}

void GameManager::Clear()
{
	//clear executors
	while (m_Executors.empty() == false)
	{
		//tips: exec quit in Executor::~()
		delete m_Executors.back();
	}
}

void GameManager::UpdateExecutors()
{
	for (auto excutor : m_Executors)
	{
		excutor->Execute();
	}
}