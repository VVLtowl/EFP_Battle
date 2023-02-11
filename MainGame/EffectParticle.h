#pragma once
#include "GameObject.h"
#include "model.h"

class EffectParticle: 
	public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	EffectParticle();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class Polygon2D* m_Polygon2D;
	//class Polygon2DMany* m_Polygon2D;

	/*********************************************************
	* @brief	customize
	********************************************************/
private:
	float m_CountDown;
	D3DXVECTOR3 m_Velocity;
	D3DXVECTOR3 m_Acceleration;
public:
	void SetUpPhysics(const D3DXVECTOR3& startPos, const D3DXVECTOR3& dir);
};
