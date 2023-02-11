#pragma once
#include "GameObject.h"
#include "RPS.h"

//base
class ButtonObject :
    public GameObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	ButtonObject(const D3DXVECTOR2& size);
	virtual ~ButtonObject();

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
private:
	class Button* m_Button;
	class BoxCollider* m_BoxCollider;
	class LookAtCamera* m_LookAtCamera;
public:
	const Collision::AABB GetAABB() const;
	class Button* GetButton();

	/*********************************************************
	* @brief	customize
	********************************************************/
protected:
	class ButtonHelper* m_ButtonHelper;
	D3DXVECTOR2 m_ImageSize;
};


class ButtonHand :
	public ButtonObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	ButtonHand(class Piece* piece);
	~ButtonHand();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/


	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	class UIRPS* m_UIRPS;
	class Piece* TargetPiece;

	//hand type
	HandType Type;
	void SetHandType(HandType type);

	//for click event
	void SetPieceHand();
};

class ButtonActSelection :
	public ButtonObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	ButtonActSelection(class Piece* piece);
	~ButtonActSelection();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/


	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	class UIActSelection* m_UIActSelection;
	class Piece* m_TargetPiece;
};

