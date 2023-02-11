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
	* @brief	コンポネント
	********************************************************/
public:
	class Polygon2D* Image;

	/*********************************************************
	* @brief	customize
	********************************************************/
};

