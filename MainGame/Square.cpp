#include "main.h"
#include "Character.h"
#include "Piece.h"
#include "Square.h"

#include "SquareType.h"


Square::EnterResult Square::CheckEnter(Piece* enterPiece) const
{
	if (enterPiece->m_Camp == CampType::BAD)
	{
		//bad man can't enter the square that has piece
		if (m_Piece != nullptr)
		{
			return EnterResult::CAN_NOT_ENTER;
		}

		//bad man escape from door
		if (Type::GOAL == m_Type)
		{
			return EnterResult::ESCAPE;
		}
	}
	else if (enterPiece->m_Camp == CampType::GOOD)
	{

		//good man can't enter goal square
		if (Type::GOAL == m_Type)
		{
			return EnterResult::CAN_NOT_ENTER;
		}

		if (m_Piece)
		{
			if (m_Piece->m_Camp == CampType::BAD)
				//good man can catch bad man
			{
				return EnterResult::CATCH_BAD;
			}
			else
				//good man can't enter the square that has piece except bad man
			{
				return EnterResult::CAN_NOT_ENTER;
			}
		}
	}

	return EnterResult::NORMAL_ENTER;
}
