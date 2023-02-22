#pragma once
#include "GameObject.h"

const D3DXVECTOR2 UI_SQUARESELECTION_SIZE = { 1.0f,1.0f };

class UISquareSelection :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UISquareSelection();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class Polygon2D* m_Polygon2D;
public:
	class Polygon2D* GetPolygon2D() { return m_Polygon2D; };

	/*********************************************************
	* @brief	customize
	********************************************************/
};

