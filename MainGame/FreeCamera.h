#pragma once
#include "GameObject.h"

class FreeCamera :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	FreeCamera();
	~FreeCamera();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class Camera* m_Camera;
	class Polygon3D* m_Polygon3D;
	//class LookAt* m_LookAt;
	//class TransformAnime* m_TransformAnime;
public:
	class Camera* GetCamera() { return m_Camera; };

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	class GameObject* m_HintObject;
	//void SetLookAt(class Transform3D* target);
};

