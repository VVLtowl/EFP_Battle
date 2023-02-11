#pragma once
#include "Judgement.h"
#include "Behaviour.h"

class JudgementBehaviour :
	public Behaviour
{
public:
	JudgementBehaviour(class Judgement* j, bool isOnce = false);

protected:
	Judgement* m_OwnerJudgement;
};





class JudgementInputPlayMode:
	public JudgementBehaviour
{
public:
	JudgementInputPlayMode(class Judgement* j) :JudgementBehaviour(j) {};
	std::string Name() override { return "JudgementInputPlayMode"; };

	void Start()override;
	void Update()override;

public:
	bool SetUpMode=false;
};

class JudgementWaitPlayersFinish :
	public JudgementBehaviour
{
public:
	JudgementWaitPlayersFinish(class Judgement* j) :JudgementBehaviour(j) {};
	std::string Name() override { return "JudgementWaitPlayersFinish"; };

	void Reset();
	void Update()override;

public:
	int FinishPlayerNum = 0;
	int TargetPlayerNum = 0;

	int PrintRowID = -1;
	void PrintCount(std::string info);
};

class JudgementWaitPiecesFinishCommand :
	public JudgementBehaviour
{
public:
	JudgementWaitPiecesFinishCommand(class Judgement* j) :JudgementBehaviour(j) {};
	std::string Name() override { return "JudgementWaitPiecesFinishCommand"; };

	void CheckPiecesOrder();
	void Reset(	std::function<void()> cmdEvent = []() {; });
	void Update()override;

public:
	enum class State
	{
		CHECK_COMMAND_PIECE,
		WAIT_PIECE_FINISH_COMMAND,
		PIECE_FINISH_COMMAND,
		ALL_PIECES_FINISH,
	};
	State WaitState = State::CHECK_COMMAND_PIECE;
	std::function<void()> OnCommand = []() {; };
	

	//for iterate piece
	std::list<class Piece*> PieceActOrder;
	std::list<class Piece*>::iterator PieceIterator;
	std::list<class Piece*>::iterator PieceEnd;

	int FinishPieceNum = 0;
	int TargetPieceNum = 0;
	int PrintRowID = -1;
	void PrintCount(std::string info);
};

class JudgementCommandShowPieces :
	public JudgementBehaviour
{
public:
	JudgementCommandShowPieces(class Judgement* j) :JudgementBehaviour(j) {};
	std::string Name() override { return "JudgementCommandShowPieces"; };

	void Reset(
		int duration,
		int targetCount,
		std::function<void()> cmdEvent = []() {; });
	void Update()override;

public:
	std::function<void()> OnCommand = []() {; };

	float FrameCount = 0;
	float Duration = 0;//piece entry duration

	int FinishCount = 0;
	int TargetCount = 0;//piece num

	std::unordered_map<int,class Piece*>::iterator PieceIterator;
	std::unordered_map<int,class Piece*>::iterator PieceEnd;
};



