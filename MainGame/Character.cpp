#include "main.h"
#include "BehaviourFactory.h"
#include "Character.h"



Character::Character(Piece* piece)
{
	//default fixed behaviour
	{
		//FixedBHs[SHOW_BUTTON_HAND] = BehaviourFactory::CreatePBH<PBH_ShowButtonHandPanel>(piece);
		//FixedBHs[INPUT_HAND] = BehaviourFactory::CreatePBH<PBH_InputHand>(piece);
		//FixedBHs[CHECK_ACTPOINT] = BehaviourFactory::CreatePBH<PBH_CheckActPoint>(piece);
	}

	//default act behaviour
	{
		//ActBHs.emplace_back(BHID_PLAYER_MOVE);
		//ActBHs.emplace_back(BHID_PLAYER_THEN_ACT);
		//ActBHs.emplace_back(BHID_PLAYER_END_ACT);
	}
}

Character::~Character()
{
	for (int i = 0; i < FIXED_BHID_MAX; i++)
	{
		FixedBHs[i] = nullptr;
	}
	ActBHs.clear();
	SpecialBHs.clear();
}

#pragma region ========== bad ==========
Character_Bad::Character_Bad(Piece* piece) :
	Character(piece)
{
	//default bad act behaviour
	{
		//ActBHIDs.emplace_back(BHID_PLAYER_);
	}
}

NormalBad::NormalBad(Piece* piece) :
	Character_Bad(piece)
{
	//fixed
	{

	}

	//act
	{

	}

	//special
	{

	}
}

#pragma endregion


#pragma region ========== good ==========
Character_Good::Character_Good(Piece* piece) :
	Character(piece)
{
	//default good act behaviour
	{
		//ActBHIDs.emplace_back(BHID_PLAYER_);
	}
}

NormalGood::NormalGood(Piece* piece) :
	Character_Good(piece)
{
	//fixed
	{

	}

	//act
	{

	}

	//special
	{

	}
}
#pragma endregion

Character* Character::Create(CharacterType type)
{
	switch (type)
	{
	case CharacterType::NORMALBAD:
		return new NormalBad();

	case CharacterType::NORMALGOOD:
		return new NormalGood();
	}
	return nullptr;
}
