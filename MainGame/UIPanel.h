#pragma once
#include "GameObject.h"
#include <list>


class UIPanel:
	public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UIPanel();
	virtual ~UIPanel();
	virtual void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/

	/*********************************************************
	* @brief	customize
	********************************************************/
};



