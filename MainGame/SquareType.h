#pragma once

#include "State.h"

class Square;

class SquareType :
	public State
{

public:
	SquareType(Square* square);
	virtual ~SquareType() {};

protected:
	Square* m_Square;

public:
	virtual void OnPieceEnter() {};
	virtual void OnPieceStay() {};
	virtual void OnPieceExit() {};
};



class SquareType_Normal :
	public SquareType
{
public:
	SquareType_Normal(Square* square) :
		SquareType(square) {};
};

class SquareType_Goal :
	public SquareType
{
public:
	SquareType_Goal(Square* square);
	void OnPieceEnter()override {};
};
