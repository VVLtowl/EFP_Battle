#pragma once
#include <vector>
using namespace std;

#pragma region ========== map ==========

const int MAP_X_MAX = 9;
const int MAP_Y_MAX = 8;

class Map
{
public:
};


#pragma region ========== square ==========
class Square
{
public:
	enum DirectionID
	{
		UP,
		RIGHT,
		DOWN,
		LEFT,
	};
public:
	vector<Square*> m_Neighbor;
	vector<Square*> m_Accessable;
};
#pragma endregion

#pragma endregion






#pragma region ========== piece ==========
class Piece
{
public:
#pragma region ========== state ==========
	class State
	{

	};
#pragma endregion
public:
	Square* m_Square;

public:
	bool TryMoveTo(Square* sqr);
};
#pragma endregion



