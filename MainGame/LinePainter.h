#pragma once
#include "GameObject.h"
#include "Singleton.h"
#include <list>

class LinePainter :
    public GameObject,
	public Singleton<LinePainter>
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	LinePainter();
	~LinePainter();
	void UpdateGameObject()override;
	

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	std::list<class DrawLine*> m_DrawLines;
public:
	void MakeLine(DrawLineDescription desc);

	/*********************************************************
	* @brief	customize
	********************************************************/
};

