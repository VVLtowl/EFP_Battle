#pragma once


class PlayerPerformHelper
{
public:
	enum State
	{
		START_SHOW_UI,
		SHOW_UI,
		START_PBH,
		PBH,
		START_CLEAR_UI,
	};
public:
	State RPSState;
};

