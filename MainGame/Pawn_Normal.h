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
	* @brief	�R���|�l���g
	********************************************************/
public:
	class Polygon2D* PieceImage;
	class LookAtCamera* LookAtCmr;
 
	/*********************************************************
	* @brief	customize
	********************************************************/
};

