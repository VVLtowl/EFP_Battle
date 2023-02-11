#pragma once
#include "GameObject.h"

const D3DXVECTOR2 UI_RPS_SIZE = { 0.5f,0.5f };

class UIRPS :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UIRPS();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class Polygon2D* m_RPSUI;
public:
	class Polygon2D* GetPolygon2D() { return m_RPSUI; };

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	void SetRPS(int type);
};

