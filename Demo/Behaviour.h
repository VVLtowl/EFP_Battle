#pragma once
#include "main.h"

#pragma region ========== event ==========

class Event
{
public:
	virtual void Excute();
};



#pragma endregion


#pragma region ========== behaviour ==========

enum BehaviourID
{
	BHID_STEP_TURN_START,
	BHID_STEP_RPS,
	BHID_STEP_CALCULATE_ACT_POINT,
	BHID_STEP_BAD_MAN_ACT,
	BHID_STEP_GOOD_MAN_ACT,
	BHID_STEP_TURN_END,
	BHID_STEP_TEST_INSERT,

	BHID_PLAYER_NULL,
	BHID_PLAYER_CLEAR_POINT,
	BHID_PLAYER_INPUT_HAND, //
	BHID_PLAYER_SELECT_ACT,
	BHID_PLAYER_UPDATE_STATE,
	BHID_PLAYER_OPEN_STATUS,
	BHID_PLAYER_CAUGHT,
	BHID_PLAYER_GET_TOOL,

	BHID_PLAYER_MOVE,
	BHID_PLAYER_GIVE_TOOL,
	BHID_PLAYER_CATCH,
	BHID_PLAYER_STEAL_TOOL,
	BHID_PLAYER_USE_TOOL,
	BHID_PLAYER_THEN_ACT,
	BHID_PLAYER_END_ACT,
};



class Behaviour
{
protected:
	enum State
	{
		START,
		UPDATE,
		END,
		NEXT,

		MAX,
	};
	State m_State{ START };

private:
	stack<Event*> m_StartEvent;
	stack<Event*> m_EndEvent;
	Behaviour* m_Next = nullptr;

protected:
	virtual void MainStart();
	virtual void Update();
	virtual void MainEnd();
	void NextState();
protected:
	void PrintState(string info);

public:
	int m_ID;

public:
	void AddStartEvent(Event* ev);
	void AddEndEvent(Event* ev);

	void SetNext(Behaviour* next);

	void Update();
};

#pragma region ========== judgement behaviour ==========

class JudgementBehaviour :public Behaviour
{
protected:
	Judgement* m_OwnerJudgement;
public:
	void SetJudgement(Judgement* jud) { m_OwnerJudgement = jud; };
};

class JBH_TurnStart :public JudgementBehaviour
{
private:
	list<Player*>::iterator m_Player;

private:
	void MainStart()override;
	void Update()override;
public:
	JBH_TurnStart() { m_ID = BHID_STEP_TURN_START; };
};

class JBH_RPS :public JudgementBehaviour
{
private:
	int m_ReadyCount;
	list<Player*>::iterator m_Player;

private:
	void MainStart()override;
	void Update()override;
public:
	JBH_RPS() :m_ReadyCount(0) { m_ID = BHID_STEP_RPS; };
};

class JBH_CalculateActPoint :public JudgementBehaviour
{
private:
	void Update()override;
	
public:
	JBH_CalculateActPoint() { m_ID = BHID_STEP_CALCULATE_ACT_POINT; };
};

class JBH_GroupAct :public JudgementBehaviour
{
protected:
	int m_ActOverCount;
	int m_ActMax;
	LoopList<Player*>* m_ActGroup;
	Player* m_Player;

protected:
	void Update()override;
};

class JBH_BadManAct :public JBH_GroupAct
{
private:
	void MainStart()override;

public:
	JBH_BadManAct() { m_ID = BHID_STEP_BAD_MAN_ACT; };
};

class JBH_GoodManAct :public JBH_GroupAct
{
private:
	void MainStart()override;

public:
	JBH_GoodManAct() { m_ID = BHID_STEP_GOOD_MAN_ACT; };
};

class JBH_TurnEnd :public JudgementBehaviour
{
private:
	void Update()override;
public:
	JBH_TurnEnd() { m_ID = BHID_STEP_TURN_END; };
};

class JBH_Insert :public JudgementBehaviour
{
private:
	void Update()override
	{
		PrintState("test: Insert");
		NextState();
	}
public:
	JBH_Insert() { m_ID = BHID_STEP_TEST_INSERT; };
};
#pragma endregion








#pragma region ========== player behaviour ==========
class PieceBehaviour :public Behaviour
{
protected:
	Player* m_Player;
public:
	void SetPiece(Player* player);
};

class PBH_ClearPoint :public PieceBehaviour
{
private:
	void Update()override;

public:
	PBH_ClearPoint() { m_ID = BHID_PLAYER_CLEAR_POINT; };
};

class PBH_InputHand :public PieceBehaviour
{
private:
	void Update()override;

public:
	PBH_InputHand() { m_ID = BHID_PLAYER_INPUT_HAND; };
};

class PBH_SelectAct :public PieceBehaviour
{
private:
	list<Behaviour*>::iterator m_SelectedAct;

private:
	void MainStart()override;
	void Update()override;

public:
	PBH_SelectAct() { m_ID = BHID_PLAYER_SELECT_ACT; };
};

class PBH_UpdateState :public PieceBehaviour
{
private:
	void Update()override;

public:
	PBH_UpdateState() { m_ID = BHID_PLAYER_UPDATE_STATE; };

};

class PBH_OpenStatus :public PieceBehaviour
{
private:
	void Update()override { NextState(); };

public:
	PBH_OpenStatus() { m_ID = BHID_PLAYER_OPEN_STATUS; };
};

class PBH_Caught :public PieceBehaviour
{
private:
	void Update()override { NextState(); };

public:
	PBH_Caught() { m_ID = BHID_PLAYER_CAUGHT; };
};

class PBH_GetTool :public PieceBehaviour
{
private:
	void Update()override { NextState(); };

public:
	PBH_GetTool() { m_ID = BHID_PLAYER_GET_TOOL; };
};






class PBH_Move:public PieceBehaviour
{
private:
	void Update()override;

public:
	PBH_Move() { m_ID = BHID_PLAYER_MOVE; };
};

class PBH_GiveTool :public PieceBehaviour
{
private:
	void Update()override { NextState(); };

public:
	PBH_GiveTool() { m_ID = BHID_PLAYER_GIVE_TOOL; };
};

class PBH_Catch :public PieceBehaviour
{
private:
	void Update()override { NextState(); };

public:
	PBH_Catch() { m_ID = BHID_PLAYER_CATCH; };
};

class PBH_StealTool :public PieceBehaviour
{
private:
	void Update()override { NextState(); };

public:
	PBH_StealTool() { m_ID = BHID_PLAYER_STEAL_TOOL; };
};

class PBH_UseTool :public PieceBehaviour
{
private:
	void Update()override { NextState(); };

public:
	PBH_UseTool() { m_ID = BHID_PLAYER_USE_TOOL; };
};

class PBH_ThenAct :public PieceBehaviour
{
private:
	void Update()override { NextState(); };

public:
	PBH_ThenAct() { m_ID = BHID_PLAYER_THEN_ACT; };
};

class PBH_EndAct :public PieceBehaviour
{
private:
	void Update()override { NextState(); };

public:
	PBH_EndAct() { m_ID = BHID_PLAYER_END_ACT; };
};


#pragma endregion

#pragma endregion


