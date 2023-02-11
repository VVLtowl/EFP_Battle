#pragma once
#include <list>
#include "Square.h"
#include "Singleton.h"

#pragma region ========== definition ==========
const unsigned int SQUARE_X_NUM = 5;
const unsigned int SQUARE_Z_NUM = 5;
#pragma endregion

class Board:
	public Singleton<Board>
{
public:
	/*********************************************************
	* @brief	�}�X����ĂɊǗ�����
	********************************************************/
	std::unordered_map<int,Square*> Squares;
public:
	void Init();
	void Uninit();


	/*********************************************************
	* @brief	for piece game start position
	********************************************************/
public:
	std::list<Square*> BadStartSquare;
	std::list<Square*> GoodStartSquare;
public:
	void SetPieceOnBadStartSquare(class Piece* piece);
	void SetPieceOnGoodStartSquare(class Piece* piece);
private:
	Square* GetBadStartSquare();
	Square* GetGoodStartSquare();
};