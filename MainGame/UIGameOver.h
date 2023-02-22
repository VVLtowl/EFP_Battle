#pragma once
#include "GameObject.h"

const D3DXVECTOR2 UI_GAME_OVER_SIZE = { 1280.0f / 3.0f, 1280.0f / 3.0f };

class UIGameOver :
	public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	enum class ResultType
	{
		BAD_WIN,
		GOOD_WIN,
	};
	UIGameOver(ResultType result);
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class PolygonUI* m_PolygonUI;

	/*********************************************************
	* @brief	customize
	********************************************************/
};
