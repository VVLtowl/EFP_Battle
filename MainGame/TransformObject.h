#pragma once
#include "GameObject.h"

class TransformObject :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	void InitLocalPosition(const D3DXVECTOR3& localPos);
};

