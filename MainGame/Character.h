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
	* @brief	プレイヤー行為
	* @details	固定行為、行動行為、特殊追加行為
	********************************************************/
public:
	enum FixedBHID
	{
		SHOW_BUTTON_HAND,// SHOW_FINISH,
		INPUT_HAND,
		CHECK_ACTPOINT,
		//SELECT_ACT,


		//CLEAR_POINT,
		//UPDATE_STATE,

		FIXED_BHID_MAX,
	};
public:	
	class Behaviour* FixedBHs[FIXED_BHID_MAX] = {};
	std::vector<class Behaviour*> ActBHs;
	std::vector<class Behaviour*> SpecialBHs;

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
	Character_Bad(class Piece* piece=nullptr);
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

