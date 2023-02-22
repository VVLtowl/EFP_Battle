#pragma once
#include "GameObject.h"

class UIActPoint :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UIActPoint();
	~UIActPoint();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
public:
	class UIFollowWorldObject* m_FollowWorldObject;

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	class UINumbers* m_OwnUINumbers;
public:
	void StartNumberChangeAnime(int num);
};

