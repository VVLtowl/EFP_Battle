#pragma once
#include "GameObject.h"
#include <vector>

const float UI_NUMBER_INTERVAL = UI_NUMBER_SIZE.x / 2.2f;

class UINumbers :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UINumbers(LayerType layer = LAYER_SCREEN_UI);
	~UINumbers();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	std::vector<class UINumber*> m_NumberUIs;
	LayerType m_LayerType;
public:
	void SetNumber(int num);
};

