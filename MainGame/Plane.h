#pragma once
#include "GameObject.h"

class Plane:
	public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	Plane();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	�R���|�l���g
	********************************************************/
public:
	class Polygon2D* Image;

	/*********************************************************
	* @brief	customize
	********************************************************/
};

