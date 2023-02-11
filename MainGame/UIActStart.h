#pragma once
#include "GameObject.h"

class UIActStart :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UIActStart();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	�R���|�l���g
	********************************************************/
private:
	class PolygonUI* m_PolygonUI;
public:
	class PolygonUI* GetPolygonUI() { return m_PolygonUI; };

	/*********************************************************
	* @brief	customize
	********************************************************/
};

