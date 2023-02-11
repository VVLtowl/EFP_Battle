#pragma once
#include <string>
#include "Behaviour.h"
#include "Player.h"

class PlayerBehaviour:
	public Behaviour
{
public:
	PlayerBehaviour(class Player* p, bool isOnce = false) :
		Behaviour(p, isOnce),
		m_Player(p)
	{};

protected:
	class Player* m_Player;
};

class PlayerWaitSelfPiecesFinish :
	public PlayerBehaviour 
{
public:
	PlayerWaitSelfPiecesFinish(class Player* p) :PlayerBehaviour(p) {};
	std::string Name() override { return "PlayerWaitSelfPiecesFinish"; };

	void Update()override;

public:
	int FinishPieceCount = 0;
	int TargetPieceCount = 0;
	void Reset(int targetNum);
};

class PlayerWaitSelfPiecesInputHand :
	public PlayerBehaviour
{
public:
	PlayerWaitSelfPiecesInputHand(class Player* p) :PlayerBehaviour(p) {};
	std::string Name() override { return "PlayerWaitSelfPiecesInputHand"; };

	void Start()override;
	void Update()override;

public:
	enum class State
	{
		CHECK_START_PIECE,
		CHECK_NEXT_PIECE,
		WAIT_INPUT,
		FINISH,
	};
	State WaitState = State::CHECK_START_PIECE;
	bool InputFinish = false;
private:
	enum class CheckType
	{
		CHECK_FIRST,
		CHECK_NEXT,
		CHECK_PREV,
	};
	Piece* CheckEnablePiece(CheckType checkType = CheckType::CHECK_FIRST);
};

