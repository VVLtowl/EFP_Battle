#pragma once
#include "GameObject.h"
class Sphere :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	Sphere();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	�R���|�l���g
	********************************************************/
private:
	class Polygon3D* m_Polygon3D;

	/*********************************************************
	* @brief	customize
	********************************************************/
};

