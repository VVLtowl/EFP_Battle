#pragma once
#include "Singleton.h"
#include "GameState.h"

//#include "Character_NormalBad.h"
//#include "Character_NormalGood.h"
#include <list>


/*********************************************************
* @brief	�Q�[���ݒ�
********************************************************/
const int PLAYER_MAX = 6;
const int PLAYER_BAD_START_NUM = 4;
const int PLAYER_GOOD_START_NUM = 2;

/*********************************************************
* @brief	���C���Q�[���̎��s���Ǘ�����
********************************************************/
class GameManager
{
	/*********************************************************
	* @brief	manage executors
	********************************************************/
private:
	static std::list<class Executor*> m_Executors;
public:
	static void Init();
	static void Uninit();
	static void Register(class Executor* executor) { m_Executors.emplace_back(executor); };
	static void Quit(class Executor* executor) { m_Executors.remove(executor); };
	static void UpdateExecutors();


	/*********************************************************
	* @brief	main executor
	********************************************************/
public:
	static std::list<class Player*> Players;
	static std::list<class Piece*> Pieces;

	//Server
	//Client
	//Judgement
	//Board??

public:
	//game scene
	static void SetUpGameScene();
	static void CleanGameScene();


};
