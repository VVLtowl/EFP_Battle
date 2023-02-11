#pragma once
#include <list>
#include "Singleton.h"
#include "Behaviour.h"
#include "Executor.h"
#include "GameManager.h"

class BehaviourFactory
{
	/*********************************************************
	* @brief	ただ削除管理
	********************************************************/
public:
	static std::list<Behaviour*> Behaviours;

	static void Register(Behaviour* bh);
	static void Quit(Behaviour* bh);
	
	static void Clear();

public:
	template <typename ConstractBehaviourType>
	static ConstractBehaviourType* CreateEternalBH();

	template <typename ConstractBehaviourType>
	static ConstractBehaviourType* CreateOnceBH();

	template <typename ConstractBehaviourType>
	static ConstractBehaviourType* CreateJBH(class Judgement* jud, bool once = false);

	template <typename ConstractBehaviourType>
	static ConstractBehaviourType* CreatePBH(class Piece* piece,bool once = false);
};



template <typename ConstractBehaviourType>
inline ConstractBehaviourType* BehaviourFactory::CreateEternalBH()
{
	ConstractBehaviourType* bh = new ConstractBehaviourType();

	//行動は全部ゲームマネジャーに実行される
	bh->SetExecutor(GameManager::Instance());

	return bh;
}

template <typename ConstractBehaviourType>
inline ConstractBehaviourType* BehaviourFactory::CreateOnceBH()
{
	ConstractBehaviourType* bh = new ConstractBehaviourType();

	//行動は全部ゲームマネジャーに実行される
	bh->SetExecutor(GameManager::Instance());

	return bh;
}

template<typename ConstractBehaviourType>
inline ConstractBehaviourType* BehaviourFactory::CreateJBH(Judgement* jud, bool once)
{
	ConstractBehaviourType* bh;
	if (once)
	{
		bh = BehaviourFactory::CreateOnceBH<ConstractBehaviourType>();
	}
	else
	{
		bh = BehaviourFactory::CreateEternalBH<ConstractBehaviourType>();
	}
	bh->SetJudgement(jud);

	return bh;
}

template<typename ConstractBehaviourType>
inline ConstractBehaviourType* BehaviourFactory::CreatePBH(Piece* piece, bool once)
{
	ConstractBehaviourType* bh;
	if (once)
	{
		bh = BehaviourFactory::CreateOnceBH<ConstractBehaviourType>();
	}
	else
	{
		bh = BehaviourFactory::CreateEternalBH<ConstractBehaviourType>();
	}
	bh->SetPiece(piece);

	return bh;
}
