#pragma once
#include <list>
#include <vector>
#include <stack>
#include <map>
#include <string>
#include "LoopList.h"
#include "Map.h"
using namespace std;

class Judgement;
class Player;
class Game;
class Charater;
enum BehaviourID;
class Behaviour;
class PieceBehaviour;
class JudgementBehaviour;

#pragma region ========== object ==========

#pragma region ========== hand ==========
class Hand
{
public:
	enum HandType
	{
		HAND_NONE,
		HAND_ROCK,
		HAND_PAPER,
		HAND_SCISSORS,
		HAND_KING,

		HAND_MAX,
	};

public:
	static string const m_HandName[HAND_MAX];

public:
	static string const HandName(HandType type);
};
#pragma endregion

#pragma region ========== player ==========

class Player
{
public:
#pragma region ========== state ==========
	class State
	{

	};
#pragma endregion
public:
	enum FixedBehaviourID
	{
		FBH_CLEAR_POINT,
		FBH_INPUT_HAND,
		FBH_SELECT_ACT,
		PLAYER_BH_UPDATE_STATE,
		PLAYER_BH_OPEN_STATUS,
		PLAYER_BH_CAUGHT,
		PLAYER_BH_GET_TOOL,

		FBH_MAX,
	};
private:
	Behaviour* m_FixedBehaviour[FBH_MAX];
	list<Behaviour*> m_ActBehaviour;
	map<int, Behaviour*> m_SpecialBehaviour;

public:
	void SetFixedBehaviour(FixedBehaviourID id, Behaviour* bh);
	void AddActBehaviour(Behaviour* bh);
	void AddSpecialBehaviour(BehaviourID id, Behaviour* bh);
	Behaviour* GetFixedBehaviour(FixedBehaviourID id);
	list<Behaviour*>* GetActBehaviourList();
	Behaviour* GetSpecialBehaviour(BehaviourID id);


public:
	//Name
	string m_Name;

	//State
	State m_State;

	//Status
	int m_Camp;
	bool m_IsOpenStatus{ false };

	//Hand
	list<int> m_Hands;

	//Piece
	Piece m_Piece;

	//ActPoint
	int m_ActPoint{ 0 };

public:

};

#pragma endregion


#pragma region ========== character ==========
class Character
{
public:
	BehaviourID m_FixedBHID[Player::FBH_MAX]; 
	list<BehaviourID> m_ActBHIDs;

public:
	enum CampType
	{
		CAMP_BAD,
		CAMP_GOOD,
	};

public:
	int m_Camp;

public:
	Character();
};

class BadMan :public Character
{
public:
	BadMan();
};
class GoodMan :public Character
{
public:
	GoodMan();
};

#pragma endregion

#pragma region ========== judgement ==========

class Judgement
{
public:
	enum StepID
	{
		STEP_TURN_START,
		STEP_RPS,
		STEP_CALCULATE_POINT,
		STEP_BAD_ACT,
		STEP_GOOD_ACT,
		STEP_TURN_END,
	};
private:
	Game* m_Game;
	vector<Behaviour*> m_MainCycle;

public:
	LoopList<Player*> m_BadActGroup;
	LoopList<Player*> m_GoodActGroup;

private:
	void PrepareMainCycle();
	void PreparePlayer();

private:
	void JumpToBH(Behaviour* bh);

public:
	void Prepare();
	void Start();
};

#pragma endregion


#pragma region ========== game ==========

class Game
{
public:
	static Game Instance;

public:
	Behaviour* m_ExcuteBehaviour;

public:
	Judgement m_Judgement;
	list<Player*> m_Players;
	list<Behaviour*> m_Behaviours;

public:

	void PrepareGame();
	void UpdateGame();
	void EndGame();

	void SetExcuteBH(Behaviour* bh);
	void JumpToBH(Behaviour* bh, Behaviour* back);
};

#pragma endregion

#pragma endregion



#pragma region ========== game function ==========
class BehaviourFactory
{
public:	
	static JudgementBehaviour* CreateJudBehaviour(BehaviourID bhid, Judgement* jud);
	static PieceBehaviour* CreatePlayerBhaviour(BehaviourID bhid, Player* player);
	static void Clear();

	template <typename ConstractBehaviourType>
	static ConstractBehaviourType* Create();
};

template <typename ConstractBehaviourType>
class CreateBehaviour
{
public:
	ConstractBehaviourType* operator()();
};

class PlayerFactory
{
public:
	static Player* Create();
};

class RPS
{
public:
	enum ResultType
	{
		RESULT_NONE = -2,
		RESULT_LOSE = -1,
		RESULT_DRAW = 0,
		RESULT_WIN = 1,
	};
private:
	static ResultType const Result[Hand::HAND_MAX][Hand::HAND_MAX];
public:
	static ResultType Battle(Hand::HandType main, Hand::HandType rival);
};
#pragma endregion


