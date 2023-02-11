#pragma once
#include <list>
#include "State.h"
#include "SquareObject.h"

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
	int ID;
	Square* Neighbors[DIR_MAX];
	
	Piece* Piece;
	SquareObject* TargetSquareObject;
};
