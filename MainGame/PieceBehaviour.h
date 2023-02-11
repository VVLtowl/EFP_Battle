#pragma once
//#include "Player.h"
#include "Behaviour.h"
#include <list>

class PieceBehaviour :public Behaviour
{
protected:
	class Piece* m_Piece;
public:
	PieceBehaviour(class Piece* piece, bool isOnce = false):
		Behaviour(piece, isOnce),
		m_Piece(piece)
	{};
};

//once
class PieceShowEntry :
	public PieceBehaviour
{
public:
	PieceShowEntry(class Piece* p) :PieceBehaviour(p, true) {};
	std::string Name() override { return "PieceShowEntry"; };

	void Start()override;
	void Update()override;

public:
	bool Finish = false;
};

//once
class PieceShowFinishMark :
	public PieceBehaviour
{
public:
	PieceShowFinishMark(class Piece* p) :PieceBehaviour(p, true) {};
	std::string Name() override { return "PieceShowFinishMark"; };

	void Start()override;
	void Update()override;

public:
	bool Finish = false;
};

//once
class PieceShowButtonsPanel :
	public PieceBehaviour
{
public:
	PieceShowButtonsPanel(class Piece* p) :
		PieceBehaviour(p, true) {};
	std::string Name() override { return "PieceShowButtonsPanel"; };

	void Start()override;
	void Update()override;

private:
	int TargetCount = 0;
	int FinishCount = 0;
};

//once
class PieceShowHand :
	public PieceBehaviour
{
public:
	PieceShowHand(class Piece* p) :PieceBehaviour(p, true) {};
	std::string Name() override { return "PieceShowHand"; };

	void Start()override;
	void Update()override;

public:
	int FinishHandCount = 0;
	int TargetHandCount = 0;
};

//once
//give up
class PieceShowActpointChange :
	public PieceBehaviour
{
public:
	PieceShowActpointChange(class Piece* p,int targetNum) :
		PieceBehaviour(p, true) ,
		TargetNum(targetNum),
		Finish(false)
	{};
	std::string Name() override { return "PieceShowActpointChange"; };

	void Start()override;
	void Update()override;

public:
	int TargetNum;
	bool Finish;
	void StartBigger();
	void StartBack();
};

//once
class PieceShowCheckActpoint :
	public PieceBehaviour
{
public:
	PieceShowCheckActpoint(class Piece* p) :PieceBehaviour(p, true) {};
	std::string Name() override { return "PieceShowCheckActpoint"; };

	void Start()override;
	void Update()override;

public:
	struct WinEffectDesc
	{
		D3DXVECTOR3 SelfHandPos;
		D3DXVECTOR3 RivalHandPos;
	};
	std::list<class GameObject*> Effects;
	float FrameCount = 0;
	float LeastFrameCount = 0;
	float EffectDuration = 0;
	float ActpointChangeFrameCount = 0;
	int TargetActpoint = 0;

	enum class State
	{
		SHOW_EFFECT_FLY,
		WAIT_SHOW_ACTPOINT_CHANGE,
		FINISH,
	};
	State CheckState = State::SHOW_EFFECT_FLY;
};

class PieceInputHand :
	public PieceBehaviour
{
public:
	PieceInputHand(class Piece* p) :PieceBehaviour(p) {};
	std::string Name() override { return "PieceInputHand"; };

	void Start()override;
	void Update()override;
	void End()override;

public:
	enum class State
	{
		START_SHOW_UI,
		WAIT_SHOW_UI_FINISH,//wait
		INPUT,//wait
		WAIT_JUDGE_SET,//wait
		FINISH,
	};
	State InputState = State::START_SHOW_UI;
};

class PieceInput :
	public PieceBehaviour
{
public:
	PieceInput(class Piece* p) :PieceBehaviour(p) {};
	std::string Name() override { return "PieceInput"; };

	void Start()override;
	void Update()override;
	void End()override;

public:
	enum class State
	{
		START_SHOW_UI,
		WAIT_SHOW_UI_FINISH,//wait
		
		INPUT,//wait

		START_HIDE_UI,
		WAIT_HIDE_UI_FINISH,//wait
		
		WAIT_TO_CONTINUE,//wait
		FINISH,
	};
	State InputState = State::START_SHOW_UI;

public:
	void SetStartShowUIEvent(std::function<void()> showUI = []() {; })
	{
		m_OnStartShowUI = showUI;
	};
	void SetStartHideUIEvent(std::function<void()> hideUI = []() {; })
	{
		m_OnStartHideUI = hideUI;
	};
private:
	std::function<void()> m_OnStartShowUI;
	std::function<void()> m_OnStartHideUI;
};

//once
class PieceClearActPoint :
	public PieceBehaviour
{
public:
	PieceClearActPoint(class Piece* p) :PieceBehaviour(p, true) {};
	std::string Name() override { return "PieceClearActPoint"; };

	void Start()override;
};
//once
class PieceClearHand :
	public PieceBehaviour
{
public:
	PieceClearHand(class Piece* p) :PieceBehaviour(p, true) {};
	std::string Name() override { return "PieceClearHand"; };

	void Start()override;
};
