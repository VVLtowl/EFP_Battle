#pragma once
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
	PlayMode Mode;

	/*********************************************************
	* @brief	�W���b�W�����g�s��
	* @details	�Œ�s��(���C���Q�[�����[�v)�A����ǉ��s��
	********************************************************/
public:
	void Init();
	void Uninit();
	void GameStart();//in game scene

	void ShuffleAndSetPiecesToPlayers();//after client num ready
	void PrepareBoard();
	void CheckPiecesActpoint();

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
	void StartCommandPiecesClearActpoint();

	void StartWaitClientsPieceFinishAct(int pieceID);
	void StartCheckPieceMove(int pieceID);
	void StartWaitClientsPieceMove(int pieceID);
	void StartCheckPieceEscape(int pieceID);
	void StartWaitClientsPieceEscape(int pieceID);
	void StartCheckPieceCaught(int pieceID);
	void StartWaitClientsPieceCaught(int pieceID);


	void StartWaitClientsShowPiecesEntry();
	void StartWaitClientsShowPiecesHandResult();


	class JudgementInputPlayMode* BH_InputPlayMode;
	class JudgementWaitPlayersFinish* BH_WaitPlayersFinish;
	class JudgementCommandShowPieces* BH_CommandShowPieces;
	class JudgementWaitPiecesFinishCommand* BH_WaitPiecesFinishCmd;

	std::vector<class JudgementBehaviour*> m_MainLoopBehaviours;
	std::list<class JudgementBehaviour*> m_SpecialBehaviours;

	/*********************************************************
	* @brief	�Q�[���̐i�s�𐧌䂷�邽�߂̕ϐ�
	********************************************************/
public:
	//factory
	std::unordered_map<int,class Player*> Players;
	std::unordered_map<int,class Piece*> Pieces;
	std::list<class Piece*> Bads;
	std::list<class Piece*> Goods;

	//std::unordered_map<int, class Piece*>::iterator PieceIterator;
	//std::unordered_map<int, class Piece*>::iterator PieceEnd;

	//for count turn
	int TurnCount = 0;
	
	//for count finish players
	//int FinishedPlayerNum=0;
	//int TargetPlayerNum=0;

//	/*********************************************************
//	* @brief	�v���C���[�S���𑀍삷��Ƃ��g��
//	********************************************************/
//public:
//	std::list<Player*>::iterator PlayerEnd;
//	std::list<Player*>::iterator PlayerIterator;

//	/*********************************************************
//	* @brief	�q���g�pUI�I�u�W�F�N�g
//	********************************************************/
//public:
//	std::list<class UIFinishMark*> FinishMarks;
//	std::list<class UIRPS*> PlayersHands;
//	std::list<class UINumbers*> PlayersActpoints;
//	class UIActMark* ActMark;
};