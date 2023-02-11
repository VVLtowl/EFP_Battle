#pragma once
#include "GameObject.h"

class UISelect :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UISelect();
	~UISelect();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class Polygon2D* m_Polygon2D;
	class Animator* m_Animator=nullptr;
public:
	class Polygon2D* GetPolygon2D() { return m_Polygon2D; };

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	void SetHighlightAnime(bool show);
};

