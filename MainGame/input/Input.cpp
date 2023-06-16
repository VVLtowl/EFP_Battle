#include "../main.h"
#include <string>
#include <stdio.h>

#pragma region ========== keyboard ==========

void KeyboardInput::Init()
{
	memset(m_OldKeyState, 0, NUM_KEY_MAX);
	memset(m_NowKeyState, 0, NUM_KEY_MAX);
}

void KeyboardInput::Uninit()
{
}

void KeyboardInput::Update()
{
	//前回のキー状態を記録
	memcpy(m_OldKeyState, m_NowKeyState, NUM_KEY_MAX);

	/*******************************************
	最上位ビットが 1 のときはキーが押されていることを、0 のときはキーが押されていないことを示す。
	最下位ビットが 1 のときはキーがトグル状態にあることを、0 のときはトグルが解除されていることを示す。
	********************************************/
	(void)GetKeyboardState(m_NowKeyState);
}

bool KeyboardInput::IsKeyPress(BYTE keyID)
{
	//0x0の場合だけ、押されていないと判断
	return (m_NowKeyState[keyID] & 0x80);//0x80: 1000 0000
}

bool KeyboardInput::IsKeyDown(BYTE keyID)
{
	return ((m_NowKeyState[keyID] & 0x80) && !(m_OldKeyState[keyID] & 0x80));
}

bool KeyboardInput::IsKeyUp(BYTE keyID)
{
	return (!(m_NowKeyState[keyID] & 0x80) && (m_OldKeyState[keyID] & 0x80));
}


#pragma endregion

#pragma region ========== mouse ==========

void MouseInput::Init(HWND wnd)
{
	m_OldButtonState.emplace(MOUSE_LEFT, 0);
	m_OldButtonState.emplace(MOUSE_RIGHT, 0);
	m_OldButtonState.emplace(MOUSE_MID, 0);

	m_NowButtonState.emplace(MOUSE_LEFT, 0);
	m_NowButtonState.emplace(MOUSE_RIGHT, 0);
	m_NowButtonState.emplace(MOUSE_MID, 0);

	m_Window = wnd;
}

void MouseInput::Uninit()
{
	m_OldButtonState.clear();
	m_NowButtonState.clear();
}

void MouseInput::Update()
{
	for (auto pair : m_OldButtonState)
	{
		/*******************************************************************************************************
		GetAsyncKeyStateの戻り値:

		0		0000	キーが押されていない、かつ、前回のGetAsyncKeyState関数呼び出し後にキーが押されていない
		1		0001	キーが押されていない、かつ、前回のGetAsyncKeyState関数呼び出し後にキーが押されている（別アプリがGetAsyncKeyState関数を呼び出した可能性あり）
		-32768	8000	キーが押されている、かつ、前回のGetAsyncKeyState関数呼び出し後にキーが押されていない
		-32767	8001	キーが押されている、かつ、前回のGetAsyncKeyState関数呼び出し後にキーが押されている（別アプリがGetAsyncKeyState関数を呼び出した可能性あり）
		
		最上位だけ、１か０かの判断で状態を更新（最下位無視）
		*******************************************************************************************************/
		int buttonID = pair.first;
		SHORT state = GetAsyncKeyState(buttonID);

		//前回の状態を保存
		(*(m_OldButtonState.find(buttonID))).second =
			(*(m_NowButtonState.find(buttonID))).second;

		//今の状態を更新
		(*(m_NowButtonState.find(buttonID))).second = state;
	}
}

bool MouseInput::IsButtonPress(int buttonID)
{
	SHORT now = (*(m_NowButtonState.find(buttonID))).second;
	return now & 0x8000;
}

bool MouseInput::IsButtonDown(int buttonID)
{
	SHORT now = (*(m_NowButtonState.find(buttonID))).second;
	SHORT old = (*(m_OldButtonState.find(buttonID))).second;
	return (now & 0x8000) && !(old & 0x8000);
}

bool MouseInput::IsButtonUp(int buttonID)
{
	SHORT now = (*(m_NowButtonState.find(buttonID))).second;
	SHORT old = (*(m_OldButtonState.find(buttonID))).second;
	return !(now & 0x8000) && (old & 0x8000);
}

POINT MouseInput::GetMousePos()
{
	POINT pt;
	GetCursorPos(&pt);

	return pt;
}

POINT MouseInput::GetMouseWinPos()
{
	POINT pt;
	GetCursorPos(&pt);
	RECT rect;
	GetWindowRect(m_Window, &rect);
	pt.x -= rect.left + GetSystemMetrics(SM_CXDLGFRAME)*2;
	pt.y -= rect.top + GetSystemMetrics(SM_CXDLGFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);

	return pt;
}


#pragma endregion

#pragma region ========== gamepad ==========


void GamePadInput::Init(int )
{
	XInputGetState(m_UserID, &m_NowPadState);
}

void GamePadInput::Uninit()
{
	SetVibration(0, 0);
}

void GamePadInput::Update()
{
	//前回の状態を保存
	m_OldPadState = m_NowPadState;

	//今の状態を更新
	XInputGetState(m_UserID, &m_NowPadState);
}

bool GamePadInput::IsButtonPress(DWORD buttonID)
{
	DWORD buttonState = m_NowPadState.Gamepad.wButtons;
	return buttonState & buttonID;
}

bool GamePadInput::IsButtonDown(DWORD buttonID)
{
	DWORD now = m_NowPadState.Gamepad.wButtons;
	DWORD old = m_OldPadState.Gamepad.wButtons;
	return (now & buttonID) && !(old & buttonID);
}

bool GamePadInput::IsButtonUp(DWORD buttonID)
{
	DWORD now = m_NowPadState.Gamepad.wButtons;
	DWORD old = m_OldPadState.Gamepad.wButtons;
	return !(now & buttonID) && (old & buttonID);
}

float GamePadInput::GetLeftTriggerPress()
{
	return m_NowPadState.Gamepad.bLeftTrigger / 256.0f;
}

float GamePadInput::GetRightTriggerPress()
{
	return m_NowPadState.Gamepad.bRightTrigger / 256.0f;
}

bool GamePadInput::IsLeftTriggerDown()
{
	return (m_NowPadState.Gamepad.bLeftTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		&& (m_OldPadState.Gamepad.bLeftTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}

bool GamePadInput::IsRightTriggerDown()
{
	return (m_NowPadState.Gamepad.bRightTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		&& (m_OldPadState.Gamepad.bRightTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}

bool GamePadInput::IsLeftTriggerUp()
{
	return (m_NowPadState.Gamepad.bLeftTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		&& (m_OldPadState.Gamepad.bLeftTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}

bool GamePadInput::IsRightTriggerUp()
{
	return (m_NowPadState.Gamepad.bRightTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		&& (m_OldPadState.Gamepad.bRightTrigger >= XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}

POINT GamePadInput::GetLeftStickPos()
{
	return POINT{ m_NowPadState.Gamepad.sThumbLX, m_NowPadState.Gamepad.sThumbLY };
}

POINT GamePadInput::GetRightStickPos()
{
	return POINT{ m_NowPadState.Gamepad.sThumbRX, m_NowPadState.Gamepad.sThumbRY };
}

void GamePadInput::SetVibration(unsigned short leftSpeed, unsigned short rightSpeed)
{
	m_Vibration.wLeftMotorSpeed = leftSpeed;
	m_Vibration.wRightMotorSpeed = rightSpeed;
	XInputSetState(m_UserID, &m_Vibration);
}

GamePadInput::~GamePadInput()
{
	Uninit();
}

#pragma endregion

