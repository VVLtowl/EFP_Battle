#pragma once
#include "GameObject.h"


class LineCollisionTester :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	LineCollisionTester();
	~LineCollisionTester();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class DrawLine* m_DrawLine;
	
	/*********************************************************
	* @brief	customize
	********************************************************/
private:
	class GameObject* m_StartHint;
	class GameObject* m_EndHint;
	class GameObject* m_CollisionHint;
};

