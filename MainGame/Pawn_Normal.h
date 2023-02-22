#pragma once
#include "GameObject.h"

const D3DXVECTOR2 PIECE_IMAGE_SIZE = { 1,1 };

class Pawn_Normal :
	public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	Pawn_Normal();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
public:
	class Polygon2D* m_Polygon2D;
	class LookAtCamera* m_LookAtCmr;
 
	/*********************************************************
	* @brief	customize
	********************************************************/
};

