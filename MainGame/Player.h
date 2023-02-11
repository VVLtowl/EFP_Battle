#pragma once
#include "Executor.h"
#include <string>
#include <list>
#include "LoopList.h"


struct PlayerDesc
{
	bool IsAI = false;//size 4
	int PieceNum = 1;//size 4
	CampType PlayerCamp = CampType::BAD;//size 4
	int ClientID = -1;//size 4
};

class Player :
    public Executor
{
public:
	Player();
	~Player() {};

	/*********************************************************
	* @brief	プレイヤー属性
	********************************************************/
public:
	//player id (check same as client id)
	int ID = -1;

	//camp
	CampType Camp = CampType::NONE;

	//client info, use in server
	class ClientMember* TargetClient = nullptr;

	//piece
	//std::list<class Piece*> SelfPieces;
	LoopList<class Piece*> SelfPieces;

	/*********************************************************
	* @brief	player data
	********************************************************/
public:
	class Piece* NowOperatePiece = nullptr;

	/*********************************************************
	* @brief	行為
	********************************************************/
public:
	void AddSelfPiece(class Piece* piece);
	void StartIterateSelfPiecesInputHand();
	void CheckSelfPieceToInputAct(class Piece* piece);

	class PlayerWaitSelfPiecesFinish* BH_WaitSelfPieces;
	class PlayerWaitSelfPiecesInputHand* BH_WaitSelfPiecesInputHand;
};

