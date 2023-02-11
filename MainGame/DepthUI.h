#pragma once
#include "GameObject.h"
#include "Transform3D.h"
#include "PolygonUI.h"

class DepthUI :
	public	GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	DepthUI();
	~DepthUI();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class PolygonUI* m_PolygonUI;

	/*********************************************************
	* @brief	customize
	********************************************************/
private:
	int m_TestBlockID;
	bool m_Show;
};

