#pragma once
#include <functional>
#include <vector>
#include <list>
#include "Executor.h"
#include "Singleton.h"
#include "Character.h"
#include "Piece.h"
#include "Player.h"

#pragma region ========== player mode ==========

class PlayMode
{
public:
	int GetPlayerNum()const { return PlayerDescs.size(); };

public:
	
	std::vector<PlayerDesc> PlayerDescs;
};

#pragma endregion

class Judgement:
	public Singleton<Judgement>,
	public Executor
{
public:
	std::string Name() override { return "judgement"; };

public:
	PlayMode Mode;

	/*********************************************************
	* @brief	ジャッジメント行為
	* @details	固定行為(メインゲームループ)、特殊追加行為
	********************************************************/
public:
	void Init();
	void Uninit();
	void GameStart();//in game scene
	void GameEnd();//in game scene

	void ShuffleAndSetPiecesToPlayers();//after client num ready
	void SetPiecesInfoBlocks(bool show);
	void CalculatePiecesActpoint();
	void PrepareBoardAndPieces();

	void StartWaitClientsCreatePlayer();
	void StartWaitClientsSetTargetPieceNum();
	void StartWaitClientsCreatePieces();
	void StartWaitClientsLoadGameScene();

	void StartWaitClientsPrepareBoard();
	enum class ShowStepType
	{
		GAME_START,
		TURN_START,
		RPS_START,
		ACT_START,
		TURN_END,
	};
	void StartWaitClientsShowStepGameStart();
	void StartWaitClientsShowStepTurnStart();
	void StartWaitClientsShowStepRPSStart();
	void StartWaitClientsShowStepActStart();
	void StartWaitClientsShowStepTurnEnd();

	void StartWaitClientsInputRPS();
	void StartCommandShowPiecesHand();
	void StartIteratePiecesShowCheckActpoint();
	void StartIteratePiecesInputAct();
	void StartCommandPiecesClearActpoint();//instead by StartIteratePiecesClearActpoint
	void StartIteratePiecesClearActpoint();
	void StartIteratePiecesClearHand();

	void StartWaitClientsPieceFinishAct(int pieceID);
	void CheckPieceMove(int pieceID, int squareID);
	void StartWaitClientsPieceMove(int pieceID, int squareID, std::string endEventInfo, std::function<void()> endEvent= []() {; });
	void CheckPieceEscape(int pieceID);
	void StartWaitClientsPieceEscape(int pieceID,int escapeSquareID , std::string endEventInfo, std::function<void()> endEvent = []() {; });
	void CheckPieceCaught(int movePieceID, int caughtPieceID);
	void StartWaitClientsPieceCaught(int movePieceID, int caughtPieceID, int prisonRoomSquareID, std::string endEventInfo, std::function<void()> endEvent = []() {; });


	void StartWaitClientsShowPiecesEntry();
	void StartWaitClientsShowPiecesHandResult();

	void StartWaitClientsGameOver(int result);

	class JudgementInputPlayMode* BH_InputPlayMode;
	class JudgementWaitPlayersFinish* BH_WaitPlayersFinish;
	class JudgementCommandShowPieces* BH_CommandShowPieces;
	class JudgementWaitPiecesFinishCommand* BH_WaitPiecesFinishCmd;

	//std::vector<class JudgementBehaviour*> m_MainLoopBehaviours;
	//std::list<class JudgementBehaviour*> m_SpecialBehaviours;

	/*********************************************************
	* @brief	ゲームの進行を制御するための変数
	********************************************************/
public:
	//factory
	//std::unordered_map<int,class Player*> m_Players;
	//std::unordered_map<int,class Piece*> m_Pieces;
	//std::list<class Piece*> m_Bads;
	//std::list<class Piece*> m_Goods;
	std::unordered_map<int, class GameObject*> m_PieceScreenPosHints;//for calculate piece info pos


	//for count turn
	int m_TurnCount = 0;

//	/*********************************************************
//	* @brief	プレイヤー全員を操作するとき使う
//	********************************************************/
//public:
//	std::list<Player*>::iterator PlayerEnd;
//	std::list<Player*>::iterator PlayerIterator;

//	/*********************************************************
//	* @brief	ヒント用UIオブジェクト
//	********************************************************/
//public:
//	std::list<class UIFinishMark*> FinishMarks;
//	std::list<class UIRPS*> PlayersHands;
//	std::list<class UINumbers*> PlayersActpoints;
//	class UIActMark* ActMark;
};