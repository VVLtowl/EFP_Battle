#pragma once
#include "GameObject.h"

const D3DXVECTOR2 UI_RPS_START_SIZE = { 1280.0f / 3.0f, 720.0f / 3.0f };

class UIRPSStart :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UIRPSStart();
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

