#pragma once
#include "GameObject.h"


class UIActMark :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UIActMark();
	~UIActMark();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	�R���|�l���g
	********************************************************/

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	class UISelect* OwnUISelect;
};

