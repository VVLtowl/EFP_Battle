#pragma once
#include "Behaviour.h"

//give up
class OBH_PieceInit:
	public Behaviour
{
public:
	class Piece* TargetPiece;
	D3DXVECTOR3 StartPosition;
	D3DXVECTOR3 EndPosition;
	float CountDown;

	bool IsOver;
public:
	void Start()override;
	void MainUpdate()override;
	void End()override;
	int ID() override { return BHID_ANIME_PIECE_INIT; };
};

