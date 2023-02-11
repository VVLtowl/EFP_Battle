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
	* @brief	�R���|�l���g
	********************************************************/
public:
	class UIFollowWorldObject* FollowWorldObject;

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	class UINumbers* OwnUINumbers;
public:
	void StartNumberChangeAnime(int num);
};

