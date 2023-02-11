#pragma once
#include "GameObject.h"

const D3DXVECTOR2 UI_NUMBER_SIZE = { 100,100 };

class UINumber :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UINumber(LayerType layer = LAYER_SCREEN_UI);
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class PolygonUI* m_NumberUI;

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	void SetNumber(int num);
};

