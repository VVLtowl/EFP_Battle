#pragma once
#include "GameObject.h"

class UIThinkMark :
    public GameObject
{
	/*********************************************************
	 * @brief	gameobject interface override
	 ********************************************************/
public:
	UIThinkMark();
	~UIThinkMark();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
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

