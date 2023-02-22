#pragma once
#include "GameObject.h"
#include <vector>

const D3DXVECTOR2 UI_TURN_START_SIZE = { 1280.0f / 3.0f, 1280.0f / 3.0f };

class UITurnStart :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UITurnStart();
	~UITurnStart();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class PolygonUI* m_TurnUI;

	/*********************************************************
	* @brief	customize
	********************************************************/
private:
	class UINumbers* m_NumbersUI;
public:
	void SetNumber(int num);
};

