#pragma once
#include "GameObject.h"
#include "RPS.h"


class UIHand :
	public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UIHand();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class PolygonUI* m_HandImage;
public:
	class UIFollowWorldObject* FollowWorldObject;
	class PolygonUI* GetPolygonUI() { return m_HandImage; };

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	HandType Type = HandType::NONE;
	void SetHandType(HandType type);
};

