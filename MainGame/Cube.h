#pragma once
#include "GameObject.h"
class Cube :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	Cube();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class Polygon3D* m_Polygon3D;
	class BoxCollider* m_BoxCollider;

	/*********************************************************
	* @brief	customize
	********************************************************/
};

