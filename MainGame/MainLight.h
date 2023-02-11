#pragma once
#include "GameObject.h"

class MainLight:
	public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	MainLight();
	~MainLight();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class MyLight* m_MyLight;
public:
	class MyLight* GetMyLight() { return m_MyLight; };

	/*********************************************************
	* @brief	customize
	********************************************************/
private:
	int m_TestBlockID;
	bool m_CanMove;
};

