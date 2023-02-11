#include "RPS.h"

std::string const Hand::m_HandName[(int)HandType::MAX] =
{
{"0_nothing"},
{"1_rock"},
{"2_paper"},
{"3_scissors"},
{"4_king"},
};

ResultType const RPS::Result[(int)HandType::MAX][(int)HandType::MAX] =
{																			//self hand
	{{ResultType::NONE},{ResultType::NONE},{ResultType::NONE},{ResultType::NONE},{ResultType::NONE},},//none
	{{ResultType::NONE},{ResultType::DRAW},{ResultType::LOSE},{ResultType::WIN},{ResultType::LOSE},},//rock
	{{ResultType::NONE},{ResultType::WIN},{ResultType::DRAW},{ResultType::LOSE},{ResultType::LOSE},},//paper
	{{ResultType::NONE},{ResultType::LOSE},{ResultType::WIN},{ResultType::DRAW},{ResultType::LOSE},},//scissors
	{{ResultType::NONE},{ResultType::WIN},{ResultType::WIN},{ResultType::WIN},{ResultType::WIN},},//king
};