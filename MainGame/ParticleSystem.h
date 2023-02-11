#pragma once
#include "GameObject.h"
#include <list>

const unsigned int PARTICLE_LIFE_COUNT = 120;

struct PARTICLE
{
	float LifeCountDown = PARTICLE_LIFE_COUNT;
	D3DXVECTOR3 Velocity = V3_ZERO;
	D3DXVECTOR3 Acceleration = V3_ZERO;
	D3DXVECTOR3 Position = V3_ZERO;
	D3DXVECTOR3 Scale = V3_ONE;
	D3DXVECTOR4 Color = V4_ONE;
};

class ParticleSystem :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	ParticleSystem();
	virtual void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
public:
	class Polygon2DMany* m_Polygon2DMany;
	//class Polygon2D* m_Polygon2D;

	/*********************************************************
	* @brief	customize
	********************************************************/
private:
	// @brief	発射情報
	int m_Duration;
	int m_FrameCount;
	int m_ShootTimes;
	int m_ShootCount;
	int m_FinishCount;//INT_MAX: infinity loop
	class TransformObject* m_Shooter;
public:
	class Transform3D* GetShooter();

	// @brief	パーティクル管理
	std::list<PARTICLE*> m_Particles;
	std::list<PARTICLE*> m_DeadParticles;
};

