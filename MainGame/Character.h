#pragma once
#include <vector>


enum class CharacterType
{
	NORMALBAD,
	NORMALGOOD,
};

//base
class Character
{
	/*********************************************************
	* @brief	�v���C���[�s��
	* @details	�Œ�s�ׁA�s���s�ׁA����ǉ��s��
	********************************************************/
public:	
	//todo
	//make button hand panel
	//make button actselection panel

	class PieceClearActPoint* BH_ClearActpoint;
	class PieceClearHand* BH_ClearHand;
	class PieceCalculateCanGoSquare* BH_CalculateSquare;

	//check move event
	//check escape event
	//check caught event
	
	std::vector<class Behaviour*> m_ActBHs;
	std::vector<class Behaviour*> m_SpecialBHs;

public:
	Character(class Piece* piece = nullptr);
	virtual ~Character();

public:
	//create character by id
	static Character* Create(CharacterType type);
};



#pragma region ========== bad ==========
//base
class Character_Bad :
	public Character
{
public:
	Character_Bad(class Piece* piece = nullptr);
};

class NormalBad :
	public Character_Bad
{
public:
	NormalBad(class Piece* piece = nullptr);
};

#pragma endregion

#pragma region ========== good ==========
//base
class Character_Good :
	public Character
{
public:
	Character_Good(class Piece* piece = nullptr);
};

class NormalGood :
	public Character_Good
{
public:
	NormalGood(class Piece* piece = nullptr);
};

#pragma endregion

