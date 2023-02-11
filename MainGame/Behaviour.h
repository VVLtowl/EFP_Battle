#pragma once
#include <stack>
#include <string>
#include <functional>
#include "BehaviourFactory.h"

enum BehaviourOperateID
{
	BH_CHANGE,
	BH_JUMP,
};

enum BehaviourID
{
	BHID_STEP_GAME_START, BHID_JUD_ENTRY_PIECES ,BHID_JUD_SHOW_GAME_START,BHID_JUD_SHOW_TURN_START,
	BHID_STEP_RPS,BHID_JUD_SHOW_RPS_START,BHID_JUD_WAIT_PLAYERS_RPS,BHID_JUD_SHOW_PLAYERS_HAND, 
	BHID_STEP_ACT_POINT,BHID_JUD_WAIT_PLAYERS_CHECK_ACTPOINT,
	BHID_STEP_BAD_MAN_ACT, BHID_JUD_SHOW_BAD_ACT_START,BHID_JUD_WAIT_BAD_PLAYERS_ACT,
	BHID_STEP_GOOD_MAN_ACT, BHID_JUD_SHOW_GOOD_ACT_START, BHID_JUD_WAIT_GOOD_PLAYERS_ACT,
	BHID_STEP_TURN_END,
	BHID_STEP_TEST_INSERT,

	BHID_PLAYER_NULL,
	BHID_PLAYER_RPS,//once
	BHID_PLAYER_SHOW_FINISH,//once
	BHID_PLAYER_SHOW_BUTTON_HAND_PANEL,
	BHID_PLAYER_INPUT_HAND, //
	BHID_PLAYER_CHECK_ACTPOINT,
	BHID_PLAYER_SELECT_ACT,
	BHID_PLAYER_CLEAR_POINT,
	
	//BHID_PLAYER_UPDATE_STATE,
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

	//BHID_ANIME_PIECE_INIT,
	//BHID_ANIME_PIECE_MOVE,
};

class Executor;

class Behaviour
{
	friend Executor;
protected:
	enum State
	{
		START,
		UPDATE,
		END,

		MAX,
	};
	int m_State = START;

public:
	Behaviour(class Executor* executor, bool isOnce = false);
	virtual ~Behaviour();
	void MainUpdate();

protected:
	virtual void Start() {};
	virtual void Update() { NextState(); };
	virtual void End() {};
	virtual int ID() { return -1; };
	virtual std::string Name() { return "nameless"; };

protected:
	void Print(std::string info);
	void NextState() { m_State++; };
	void ResetState() { m_State = START; };

private:
	Behaviour* m_Next = nullptr;
public:
	void SetNext(Behaviour* next){ m_Next = next; };

public:
	void SwitchToNext(Behaviour* bh);
	void JumpTo(Behaviour* jump); 

protected:
	Executor* m_Executor = nullptr;
public:
	void SetExecutor(Executor* ex) { m_Executor = ex; };//

protected:
	std::function<void()> m_EndEvent;
public:
	void SetEndEvent(std::function<void()> func) { m_EndEvent = func; };

protected:
	bool m_IsOnceBehaviour = false;
	int m_PrintRowID = -1;
};

