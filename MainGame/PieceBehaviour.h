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
class PieceShowMove :
	public PieceBehaviour
{
public:
	PieceShowMove(class Piece* p) :
		PieceBehaviour(p, true) {};
	std::string Name() override { return "PieceShowMove"; };

	void Reset(const D3DXVECTOR3& targetPos);
	void Update()override;

private:
	bool m_Finish;
};

//once
class PieceShowCaught :
	public PieceBehaviour
{
public:
	PieceShowCaught(class Piece* p) :
		PieceBehaviour(p, true) {};
	std::string Name() override { return "PieceShowCaught"; };

	void Reset(const D3DXVECTOR3& targetPos);
	void Update()override;

private:
	bool m_Finish;
};

//once
class PieceShowEscape :
	public PieceBehaviour
{
public:
	PieceShowEscape(class Piece* p) :
		PieceBehaviour(p, true) {};
	std::string Name() override { return "PieceShowEscape"; };

	void Reset(const D3DXVECTOR3& targetPos);
	void Update()override;

private:
	bool m_Finish = false;
};

//once
class PieceShowButtonsPanel_ButtonSquare :
	public PieceBehaviour
{
public:
	PieceShowButtonsPanel_ButtonSquare(class Piece* p) :
		PieceBehaviour(p, true) {};
	std::string Name() override { return "PieceShowButtonsPanel_ButtonSquare"; };

	void Start()override;
	void Update()override;

private:
	int TargetCount = 0;
	int FinishCount = 0;
};
//once
class PieceHideButtonsPanel_ButtonSquare :
	public PieceBehaviour
{
public:
	PieceHideButtonsPanel_ButtonSquare(
		class Piece* p,
		const D3DXVECTOR3& targetPos) :
		PieceBehaviour(p, true), 
		m_TargetPos(targetPos) {};
	std::string Name() override { return "PieceHideButtonsPanel_ButtonSquare"; };

	void Start()override;
	void Update()override;

private:
	D3DXVECTOR3 m_TargetPos;
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
	PieceShowCheckActpoint(class Piece* p,int actPoint) :
		PieceBehaviour(p, true),
		m_TargetActpoint(actPoint)
	{};
	std::string Name() override { return "PieceShowCheckActpoint"; };

	void Start()override;
	void Update()override;

public:
	struct WinEffectDesc
	{
		D3DXVECTOR3 SelfHandPos;
		D3DXVECTOR3 RivalHandPos;
	};
	std::list<class GameObject*> m_Effects;
	float m_FrameCount = 0;
	float m_ActpointChangeFrameCount = 0;
	int m_TargetActpoint = 0;

	enum class State
	{
		WAIT_CAMERA_MOVE_FINISH,
		SHOW_EFFECT_FLY,
		FINISH,
	};
	State m_CheckState = State::WAIT_CAMERA_MOVE_FINISH;
};

class PieceInputHand :
	public PieceBehaviour
{
public:
	PieceInputHand(class Piece* p) :PieceBehaviour(p) {};
	std::string Name() override { return "PieceInputHand"; };

	void Start()override;
	void Update()override;

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

	//void Start()override;
	void Update()override;
	//void End()override;

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
	void Reset(
		std::function<void()> showUI = []() {; },
		std::function<void()> hideUI = []() {; });
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

//test
//once
class PieceClearActPoint :
	public PieceBehaviour
{
public:
	PieceClearActPoint(class Piece* p) :PieceBehaviour(p, true) {};
	std::string Name() override { return "PieceClearActPoint"; };

	void Start()override;
};
//test
//once
class PieceClearHand :
	public PieceBehaviour
{
public:
	PieceClearHand(class Piece* p) :PieceBehaviour(p, true) {};
	std::string Name() override { return "PieceClearHand"; };

	void Start()override;
};

#pragma region ========== calculate actpoint ==========
//base
class PieceCalculateActpoint :
	public PieceBehaviour
{
public:
	PieceCalculateActpoint(class Piece* p) :PieceBehaviour(p) {};
	std::string Name() override { return "PieceCalculateActpoint"; };
};

class CalculateActpoint_Normal :
	public PieceCalculateActpoint
{
public:
	CalculateActpoint_Normal(class Piece* p) :PieceCalculateActpoint(p) {};
	std::string Name() override { return "CalculateActpoint_Normal"; };

	void Start()override;
};

#pragma endregion

#pragma region ========== calculate can go square ==========
//base
class PieceCalculateCanGoSquare :
	public PieceBehaviour
{
public:
	PieceCalculateCanGoSquare(class Piece* p) :PieceBehaviour(p) {};
	std::string Name() override { return "PieceCalculateCanGoSquare"; };

public:
	void ClearCanGoSquares() { m_CanGoSquares.clear(); };
	std::list<class Square*>* GetCanGoSquares() { return &m_CanGoSquares; };
protected:
	std::list<class Square*> m_CanGoSquares;
};

class CalculateSquare_Normal:
	public PieceCalculateCanGoSquare
{
public:
	CalculateSquare_Normal(class Piece* p) :PieceCalculateCanGoSquare(p) {};
	std::string Name() override { return "CalculateSquare_Normal"; };

	void Start()override;
};
#pragma endregion

