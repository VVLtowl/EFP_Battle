#pragma once
#include "GameObject.h"


class UIActMark :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UIActMark(class Transform3D* targetTrans);
	~UIActMark();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
public:
	class UIFollowWorldObject* m_FollowWorldObject;
	
	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	class UIMark* m_UIMark;
};

