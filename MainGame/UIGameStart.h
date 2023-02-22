#pragma once
#include "GameObject.h"

const D3DXVECTOR2 UI_GAME_START_SIZE = { 1280.0f / 3.0f, 1280.0f / 3.0f };

class UIGameStart :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UIGameStart();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class PolygonUI* m_PolygonUI;

	/*********************************************************
	* @brief	customize
	********************************************************/
};

