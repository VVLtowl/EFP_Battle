#pragma once

#include <Windows.h>
#include <Xinput.h>
#include <unordered_map>

#pragma comment(lib,"xinput.lib")

#define FIRSTTOOL_API __declspec(dllexport)

/*********************************************************
* @brief		入力説明
* @details		・キーボード
*					初期化
*					更新処理
*					終了処理
*					ボタン押されている状態の確認
*					ボタン押された瞬間の確認
*					ボタン離された瞬間の確認
*				・マウス
*					初期化
*					更新処理
*					終了処理
*					ボタン押されている状態の確認
*					ボタンおされた瞬間の確認
*					ボタン離された瞬間の確認
*					マウス座標の取得
*				・ゲームパット
*					初期化
*					更新処理
* 					終了処理
*					ボタン押されている状態の確認
* 					ボタンおされた瞬間の確認
* 					ボタン離された瞬間の確認
*					トリガー押されている状態の確認
* 					トリガーおされた瞬間の確認
* 					トリガー離された瞬間の確認
*					スティック倒れ座標の取得
*					バイブレーションの設定
********************************************************/

#pragma region ========== keyboard ==========
//========== keyboard button macro ==========
static const int NUM_KEY_MAX = 256;

static const int KEY_A = 'A';
static const int KEY_B = 'B';
static const int KEY_C = 'C';
static const int KEY_D = 'D';
static const int KEY_E = 'E';
static const int KEY_F = 'F';
static const int KEY_G = 'G';
static const int KEY_H = 'H';
static const int KEY_I = 'I';
static const int KEY_J = 'J';
static const int KEY_K = 'K';
static const int KEY_L = 'L';
static const int KEY_M = 'M';
static const int KEY_N = 'N';
static const int KEY_O = 'O';
static const int KEY_P = 'P';
static const int KEY_Q = 'Q';
static const int KEY_R = 'R';
static const int KEY_S = 'S';
static const int KEY_T = 'T';
static const int KEY_U = 'U';
static const int KEY_V = 'V';
static const int KEY_W = 'W';
static const int KEY_X = 'X';
static const int KEY_Y = 'Y';
static const int KEY_Z = 'Z';
static const int KEY_UP = VK_UP;
static const int KEY_RIGHT = VK_RIGHT;
static const int KEY_DOWN = VK_DOWN;
static const int KEY_LEFT = VK_LEFT;


//========== keyboard input class definition ==========

// @brief		キーボード入力対象
class KeyboardInput
{
public:
	// @brief	初期化
	void Init();

	// @brief	終了処理
	void Uninit();

	// @brief	更新処理
	void Update();

	/*********************************************************
	* @brief	ボタン押されている状態の確認
	* @param	[in]keyID: キーのID　例：'A'
	********************************************************/
	bool IsKeyPress(BYTE keyID);

	/*********************************************************
	* @brief	ボタン押された瞬間の確認
	* @param	[in]keyID: キーのID　例：'A'
	********************************************************/
	bool IsKeyDown(BYTE keyID);

	/*********************************************************
	* @brief	ボタン離された瞬間の確認
	* @param	[in]keyID: キーのID　例：'A'
	********************************************************/
	bool IsKeyUp(BYTE keyID);

private:
	// @brief	直前のフレームでのキー状態を格納用
	BYTE m_OldKeyState[NUM_KEY_MAX];

	// @brief	現在のフレームでのキー状態を格納用
	BYTE m_NowKeyState[NUM_KEY_MAX];
};

#pragma endregion

#pragma region ========== mouse ==========
//========== mouse button macro ==========
const int MOUSE_LEFT = VK_LBUTTON;
const int MOUSE_RIGHT = VK_RBUTTON;
const int MOUSE_MID = VK_MBUTTON;

//========== mouse input class definition ==========

// @brief		マウス入力対象
class MouseInput
{
public:
	// @brief	初期化
	void Init(HWND wnd);

	// @brief	終了処理
	void Uninit();

	// @brief	更新処理
	void Update();

	/*********************************************************
	* @brief	ボタン押されている状態の確認
	* @param	[in]buttonID: ボタンのID　例：MOUSE_LEFT
	********************************************************/
	bool IsButtonPress(int buttonID);

	/*********************************************************
	* @brief	ボタン押された瞬間の確認
	* @param	[in]buttonID: ボタンのID　
	********************************************************/
	bool IsButtonDown(int buttonID);

	/*********************************************************
	* @brief	ボタン離された瞬間の確認
	* @param	[in]buttonID: ボタンのID　
	********************************************************/
	bool IsButtonUp(int buttonID);

	/*********************************************************
	* @brief	マウス座標の取得
	********************************************************/
	POINT GetMousePos();

	/*********************************************************
	* @brief	マウスウィンドウズ座標の取得
	********************************************************/
	POINT GetMouseWinPos();

private:
	// @brief	直前のフレームでのボタン状態を格納用
	std::unordered_map<int, SHORT> m_OldButtonState;

	// @brief	現在のフレームでのボタン状態を格納用
	std::unordered_map<int, SHORT> m_NowButtonState;

	// @brief	window handle
	HWND m_Window;
};

#pragma endregion

#pragma region ========== gamepad xinput ==========

//========== gamepad xinput ==========
/*********************************************************
* @brief		xinput learning
* @details		controller state from: button, trigger, thumb
********************************************************/

//========== xinput button macro ==========
const DWORD XPAD_X = XINPUT_GAMEPAD_X;
const DWORD XPAD_A = XINPUT_GAMEPAD_A;
const DWORD XPAD_B = XINPUT_GAMEPAD_B;
const DWORD XPAD_Y = XINPUT_GAMEPAD_Y;
const DWORD XPAD_UP = XINPUT_GAMEPAD_DPAD_UP;
const DWORD XPAD_DOWN = XINPUT_GAMEPAD_DPAD_DOWN;
const DWORD XPAD_LEFT = XINPUT_GAMEPAD_DPAD_LEFT;
const DWORD XPAD_RIGHT = XINPUT_GAMEPAD_DPAD_RIGHT;
const DWORD XPAD_L1 = XINPUT_GAMEPAD_LEFT_SHOULDER;
const DWORD XPAD_R1 = XINPUT_GAMEPAD_RIGHT_SHOULDER;
const DWORD XPAD_L2 = -1;
const DWORD XPAD_R2 = 1;
const DWORD XPAD_LT = XINPUT_GAMEPAD_LEFT_THUMB;
const DWORD XPAD_RT = XINPUT_GAMEPAD_RIGHT_THUMB;

// @brief		スティック倒れ値の最大値(-32728~32727)
const unsigned short STICK_MOVE_MAX = 32728;

// @brief		バイブレーションの最大値(0~65535)
const unsigned short VIBRATION_MAX = 65535;

//========== gamepad input class definition ==========

// @brief		マウス入力対象
class GamePadInput
{
public:
	/*********************************************************
	* @brief	初期化
	* @param	[in]userID: ユーザーIDを設定（このパッド特有ID）
	********************************************************/
	void Init(int userID);

	// @brief	終了処理
	void Uninit();

	// @brief	更新処理
	void Update();

	/*********************************************************
	* @brief	ボタン押されている状態の確認
	* @param	[in]buttonID: ボタンのID　例：XPAD_LEFT
	********************************************************/
	bool IsButtonPress(DWORD buttonID);

	/*********************************************************
	* @brief	ボタン押された瞬間の確認
	* @param	[in]buttonID: ボタンのID　
	********************************************************/
	bool IsButtonDown(DWORD buttonID);

	/*********************************************************
	* @brief	ボタン離された瞬間の確認
	* @param	[in]buttonID: ボタンのID　
	********************************************************/
	bool IsButtonUp(DWORD buttonID);

	/*********************************************************
	* @brief	トリガー押されている状態の確認
	* @return	トリガー回転角度の割合
	********************************************************/
	float GetLeftTriggerPress();
	float GetRightTriggerPress();

	/*********************************************************
	* @brief	トリガー押された瞬間の確認
	********************************************************/
	bool IsLeftTriggerDown();
	bool IsRightTriggerDown();

	/*********************************************************
	* @brief	トリガー離された瞬間の確認
	********************************************************/
	bool IsLeftTriggerUp();
	bool IsRightTriggerUp();

	/*********************************************************
	* @brief	スティック倒れ座標の取得
	* @return	スティック倒れ座標xとy (-32768 から 32767)
	********************************************************/
	POINT GetLeftStickPos();
	POINT GetRightStickPos();

	/*********************************************************
	* @brief	バイブレーションの設定
	* @param	[in]leftSpeed: 左バイブレーションの強度 (0 から 65535)
	* @param	[in]rightSpeed: 右バイブレーションの強度 (0 から 65535)
	********************************************************/
	void SetVibration(unsigned short leftSpeed, unsigned short rightSpeed);

private:
	// @brief		前回のパット状態格納用
	_XINPUT_STATE m_OldPadState;

	// @brief		現在のパット状態格納用
	_XINPUT_STATE m_NowPadState;

	// @brief		バイブレーションの状態
	_XINPUT_VIBRATION m_Vibration;

	// @brief		ユーザーID
	int m_UserID{ 0 };

public:
	// @brief		意外削除の場合、デストラクタで終了処理
	~GamePadInput();
};

#pragma endregion



