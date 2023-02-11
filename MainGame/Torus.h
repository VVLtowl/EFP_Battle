#pragma once
#include "GameObject.h"

class Torus :
	public	GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	Torus();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class Polygon3D* m_Polygon3D;

	/*********************************************************
	* @brief	customize
	********************************************************/
private:
	D3DXVECTOR3 m_EulerAngle;
};

