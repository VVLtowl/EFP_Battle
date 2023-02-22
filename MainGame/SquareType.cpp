#include "main.h"
#include "Square.h"
#include "SquareType.h"

SquareType::SquareType(Square* square) :
	State(square),
	m_Square(square) 
{
};

#pragma region ========== goal square ==========

SquareType_Goal::SquareType_Goal(Square* square) :
	SquareType(square)
{
	square->m_Type = Square::Type::GOAL;
};

#pragma endregion
