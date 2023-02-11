#pragma once
#include "GameObject.h"

class ButtonsPanel :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	ButtonsPanel();
	~ButtonsPanel();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	�R���|�l���g
	********************************************************/
public:
	class ButtonGroup* m_ButtonGroup;
	class LookAtCamera* m_LookAtCmr;

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
};

