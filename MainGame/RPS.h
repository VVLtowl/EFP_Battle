#pragma once
#include <string>

enum HandID
{
	LEFT_HAND,
	RIGHT_HAND,
};


enum class HandType
{
	NONE,
	ROCK,
	PAPER,
	SCISSORS,
	KING,

	MAX,
};

enum class ResultType
{
	NONE = -2,
	LOSE = -1,
	DRAW = 0,
	WIN = 1,
};

class Hand
{
public:
	static std::string const m_HandName[(int)HandType::MAX];

public:
	static std::string const HandName(HandType type);
};

class RPS
{
private:
	static ResultType const Result[(int)HandType::MAX][(int)HandType::MAX];
public:
	static ResultType Battle(HandType main, HandType rival);
};

#pragma region ========== definition ==========
inline std::string const Hand::HandName(HandType type)
{
	return m_HandName[(int)type];
}

inline ResultType RPS::Battle(HandType main, HandType rival)
{
	return Result[(int)main][(int)rival];
}
#pragma endregion