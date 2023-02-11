#pragma once

#include "State.h"

class Square;

class SquareState :
	public State
{
public:
	Square* TargetSquare;

public:
	virtual ~SquareState() {};

public:
	virtual void OnPieceEnter() {};
	virtual void OnPieceStay() {};
	virtual void OnPieceExit() {};
};