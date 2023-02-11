#pragma once
#include "GameObject.h"
#include "RPS.h"

const D3DXVECTOR2 UI_HAND_SIZE = { 20,20 };

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

