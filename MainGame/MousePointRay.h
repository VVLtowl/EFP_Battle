#pragma once
#include "GameObject.h"
#include "Singleton.h"

#define HINT_MOUSE_RAY

class MousePointRay :
    public GameObject,
    public Singleton<MousePointRay>
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	MousePointRay();
	~MousePointRay();
	void UpdateGameObject()override;


	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class DrawLine* m_HintLine;

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	Collision::Line const GetLine();
private:
	Collision::Line m_MouseRay;
};

