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
	std::string Name() override { return "player" + std::to_string(m_ID); };

	/*********************************************************
	* @brief	プレイヤー属性
	********************************************************/
public:
	//player id (check same as client id)
	int m_ID = -1;

	//camp
	CampType m_Camp = CampType::NONE;

	//client info, use in server
	struct ClientMember* m_TargetClient = nullptr;

	//piece
	//std::list<class m_Piece*> SelfPieces;
	LoopList<class Piece*> m_SelfPieces;

	//for oterate self piece
	Piece* m_NowOperatePiece = nullptr;

	/*********************************************************
	* @brief	行為
	********************************************************/
public:
	void ClearDataInGameScene();
	void AddSelfPiece(class Piece* piece);
	void StartIterateSelfPiecesInputHand();
	void CheckSelfPieceToInputAct(class Piece* piece);

	class PlayerWaitSelfPiecesFinish* BH_WaitSelfPieces;
	class PlayerWaitSelfPiecesInputHand* BH_WaitSelfPiecesInputHand;
};

