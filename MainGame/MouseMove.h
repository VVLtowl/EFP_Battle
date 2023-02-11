#pragma once
#include "GameObject.h"
#include "Singleton.h"

class MouseMove :
    public GameObject,
    public Singleton<MouseMove>
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	MouseMove();
	~MouseMove();
	void UpdateGameObject()override;


	/*********************************************************
	* @brief	コンポネント
	********************************************************/


	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	D3DXVECTOR2 const GetSpeed() { return m_Speed; };
private:
	D3DXVECTOR2 m_Speed = V2_ZERO;
	POINT m_LastPos = { 0,0 };
};

