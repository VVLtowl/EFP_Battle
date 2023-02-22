#pragma once
#include <list>
#include "State.h"
#include "SquareObject.h"

#include "SquareType.h"

class Piece;

#pragma region ========== enum definition ==========
enum SquareDirection
{
	DIR_UP,
	DIR_RIGHT,
	DIR_DOWN,
	DIR_LEFT,

	DIR_MAX,
};
#pragma endregion


/*********************************************************
* @brief	É}ÉX
********************************************************/
class Square:
	public StateOwner
{
public:
	enum class Type
	{
		NORMAL,
		GOAL,
	};
public:
	//id
	int m_ID;

	//type
	Type m_Type = Type::NORMAL;

	//up, right, down, left square
	Square* m_Neighbors[DIR_MAX] = {nullptr};

	//the piece on this square
	Piece* m_Piece;

	//square object
	SquareObject* m_SquareObject;


	/*********************************************************
	* @brief	çsà◊
	********************************************************/
public:
	//result for show ui
	enum class EnterResult
	{
		CAN_NOT_ENTER,

		NORMAL_ENTER,
		CATCH_BAD,
		ESCAPE,
	};
	EnterResult CheckEnter(Piece* enterPiece)const;
};
