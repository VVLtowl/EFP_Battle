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



/*********************************************************
* @brief	button for input hand
********************************************************/
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

/*********************************************************
* @brief	button for select act
********************************************************/
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

/*********************************************************
* @brief	button for input move
********************************************************/
class ButtonInputMove :
	public ButtonObject
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	ButtonInputMove(class Piece* piece, class Square* square);
	virtual ~ButtonInputMove() {};
	void UpdateGameObject()override {};

	/*********************************************************
	* @brief	コンポネント
	********************************************************/


	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	class Piece* m_TargetPiece;
	class Square* m_TargetSquare;
};


class ButtonSquareSelection :
	public ButtonInputMove
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	ButtonSquareSelection(class Piece* piece,class Square* square);
	virtual ~ButtonSquareSelection();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/


	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	class UISquareSelection* m_UISquareSelection;
};

class ButtonMoveCancel :
	public ButtonInputMove
{
	/*********************************************************
	* @brief	gameobject interface override
	********************************************************/
public:
	ButtonMoveCancel(class Piece* piece, class Square* square);
	~ButtonMoveCancel();
	void UpdateGameObject()override;

	/*********************************************************
	* @brief	コンポネント
	********************************************************/
public:
	class LookAtCamera* m_LookAtCmr;

	/*********************************************************
	* @brief	customize
	********************************************************/
public:
	class UIActSelection* m_UICancel;
};

