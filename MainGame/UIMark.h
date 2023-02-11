#pragma once
#include "GameObject.h"

const D3DXVECTOR2 UI_MARK_SIZE = { 40.0f,40.0f };

class UIMark :
	public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	UIMark();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class PolygonUI* m_MarkUI;
	class Animator* m_Animator;
public:
	class PolygonUI* GetPolygonUI() { return m_MarkUI; };

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	enum MarkType
	{
		MARK_FINISH,
		MARK_ACT,
		MARK_THINK,

		MARK_MAX,
	};
	void SetMark(int type);
	int MarkType = MARK_THINK;
};