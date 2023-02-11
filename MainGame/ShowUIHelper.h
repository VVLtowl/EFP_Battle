#pragma once

#pragma region ========== step ui ==========
struct ShowUIDescripition
{
	float PositionX = SCREEN_WIDTH / 2;
	float PositionYUIInStart = -SCREEN_HEIGHT / 3.0f;
	float PositionYUIWait = SCREEN_HEIGHT / 2;
	float PositionYUIWaitStart = SCREEN_HEIGHT / 2 - 5.0f;
	float PositionYUIWaitEnd = SCREEN_HEIGHT / 2 + 5.0f;
	float PositionYUIOutEnd = SCREEN_HEIGHT + SCREEN_HEIGHT / 3.0f;

	//ui in
	float DurationUIIn = 30.0f;
	D3DXVECTOR3 PositionUIInStart() { return { PositionX,PositionYUIInStart,0 }; };
	D3DXVECTOR3 TangentUIInStart = { 0,200,0 };
	D3DXVECTOR3 TangentUIInEnd = { 0,800,0 };
	//ui wait
	float DurationUIWait = 40.0f;
	D3DXVECTOR3 PositionUIWait() { return { PositionX,PositionYUIWait ,0 }; };
	D3DXVECTOR3 PositionUIWaitStart() { return { PositionX,PositionYUIWaitStart ,0 }; };
	D3DXVECTOR3 PositionUIWaitEnd() { return { PositionX, PositionYUIWaitEnd, 0 }; };
	//ui out
	float DurationUIOut = 30.0f;
	D3DXVECTOR3 PositionUIOutEnd() { return { PositionX, PositionYUIOutEnd, 0 }; };
	D3DXVECTOR3 TangentUIOutStart = { 0,-800,0 };
	D3DXVECTOR3 TangentUIOutEnd = { 0,-20,0 };
};

class ShowUIHelper
{
public:
	ShowUIHelper() {};
	~ShowUIHelper() { UIObject = nullptr; };

	void Start();
	bool UpdateCheckFinish();

public:
	ShowUIDescripition ShowUIDesc;

private:
	virtual void StartUIIn();
	virtual void StartUIWait();
	virtual void StartUIOut();

public:
	// @brief	演出終わるかの判断
	enum State
	{
		UI_IN,
		START_WAIT,
		UI_WAIT,
		START_OUT,
		UI_OUT,
		END,
	};
	int State;

	// @brief	スタートUIオブジェクト
	// @details	演出終わったとき消える
	class GameObject* UIObject;
};
#pragma endregion

#pragma region ========== mark ui ==========
struct ShowMarkDescripition
{
	struct AnimeDesc
	{
		float Duration;
		D3DXVECTOR3 StartPosition;
		D3DXVECTOR3 StartTangent;
		D3DXVECTOR3 StartScale;
		D3DXVECTOR3 EndPosition;
		D3DXVECTOR3 EndTangent;
		D3DXVECTOR3 EndScale;
	};

	AnimeDesc ShowAniDesc=
	{
		8,
		{0,-1,0},
		{0,-100,0},
		{0,0,0},
		{0,0,0},
		{0,100,0},
		{1,1,1},
	};
	AnimeDesc ClearAniDesc=
	{
		15,
		{0,0,0},
		{0,-100,0},
		{1,1,1},
		{0,-1,0},
		{0,100,0},
		{0,0,0},
	};
};

class ShowMarkHelper
{
public:
	class GameObject* UIObject;
	ShowMarkDescripition Desc;
	virtual void StartShowUI();
	virtual void StartClearUI();

public:
	enum State
	{
		START_UI_ANIME,
		UI_ANIME,//
		END,
	};
public:
	// @brief	演出終わるかの判断
	int State;
};

#pragma endregion
