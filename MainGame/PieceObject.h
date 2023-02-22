#pragma once
#include "GameObject.h"

class PieceObject :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	PieceObject();
	~PieceObject();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class LookAtCamera* m_LookAtCamera;

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	class Pawn_Normal* m_PawnImage;
public:
	//ui transform object id
	enum class UITID
	{
		PAWN_CENTER,
		PAWN_UP_LEFT, PAWN_UP, PAWN_UP_RIGHT,
		ROOT,

		MAX,
	};
	class TransformObject* m_UITransform[(int)UITID::MAX];
public:
	void SetTexColor(const D3DXVECTOR4& cl);
	GameObject* GetUITransform(UITID uitID);
};
