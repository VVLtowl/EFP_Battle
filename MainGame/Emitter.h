#pragma once
#include "GameObject.h"

class Emitter:
	public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	Emitter();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/

	/*********************************************************
	* @brief	customize
	********************************************************/
private:
	int m_Duration;
	int m_FrameCount;

	int m_ShootTimes;
	int m_ShootCount;
	
	int m_FinishCount;
};

