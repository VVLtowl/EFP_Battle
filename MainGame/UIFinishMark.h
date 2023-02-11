#pragma once
#include "GameObject.h"

class UIFinishMark :
    public GameObject
{
    /*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UIFinishMark();
	~UIFinishMark();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	�R���|�l���g
	********************************************************/
public:
	//class PolygonUI* MarkUI;
	class UIFollowWorldObject* FollowWorldObject;

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	class UIMark* OwnUIMark;
};

