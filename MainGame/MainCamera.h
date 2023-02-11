#pragma once
#include "GameObject.h"
#include "Transform3D.h"

class Camera;

class MainCamera:
	public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	MainCamera();
	~MainCamera();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class Camera* m_Camera;
	class Polygon3D* m_Polygon3D;
	//class LookAt* m_LookAt;

public:
	class Camera* GetCamera() { return m_Camera; };

	/*********************************************************
	* @brief	customize
	********************************************************/
private:
	int m_TestBlockID;
	bool m_CanMove;
	float m_MoveSpeedMouse;
	float m_MoveSpeedKeyboard;
};

