#pragma once
#include "GameObject.h"

#pragma region ========== definition ==========
const unsigned int SQUARE_SIZE = 1;
#pragma endregion

class SquareObject :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	SquareObject();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class Polygon3D* m_Polygon3D;

	/*********************************************************
	* @brief	customize
	********************************************************/
};

