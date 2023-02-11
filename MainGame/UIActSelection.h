#pragma once
#include "GameObject.h"

const D3DXVECTOR2 UI_ACTSELECTION_SIZE = { 0.8f,0.8f };

class UIActSelection :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UIActSelection();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class Polygon2D* m_ActSelectionUI;
public:
	class Polygon2D* GetPolygon2D() { return m_ActSelectionUI; };

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	void SetActTex(int texid = TEXID_UI_MARK);
};

