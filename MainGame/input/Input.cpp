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
	//�O��̃L�[��Ԃ��L�^
	memcpy(m_OldKeyState, m_NowKeyState, NUM_KEY_MAX);

	/*******************************************
	�ŏ�ʃr�b�g�� 1 �̂Ƃ��̓L�[��������Ă��邱�Ƃ��A0 �̂Ƃ��̓L�[��������Ă��Ȃ����Ƃ������B
	�ŉ��ʃr�b�g�� 1 �̂Ƃ��̓L�[���g�O����Ԃɂ��邱�Ƃ��A0 �̂Ƃ��̓g�O������������Ă��邱�Ƃ������B
	********************************************/
	(void)GetKeyboardState(m_NowKeyState);
}

bool KeyboardInput::IsKeyPress(BYTE keyID)
{
	//0x0�̏ꍇ�����A������Ă��Ȃ��Ɣ��f
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
		GetAsyncKeyState�̖߂�l:

		0		0000	�L�[��������Ă��Ȃ��A���A�O���GetAsyncKeyState�֐��Ăяo����ɃL�[��������Ă��Ȃ�
		1		0001	�L�[��������Ă��Ȃ��A���A�O���GetAsyncKeyState�֐��Ăяo����ɃL�[��������Ă���i�ʃA�v����GetAsyncKeyState�֐����Ăяo�����\������j
		-32768	8000	�L�[��������Ă���A���A�O���GetAsyncKeyState�֐��Ăяo����ɃL�[��������Ă��Ȃ�
		-32767	8001	�L�[��������Ă���A���A�O���GetAsyncKeyState�֐��Ăяo����ɃL�[��������Ă���i�ʃA�v����GetAsyncKeyState�֐����Ăяo�����\������j
		
		�ŏ�ʂ����A�P���O���̔��f�ŏ�Ԃ��X�V�i�ŉ��ʖ����j
		*******************************************************************************************************/
		int buttonID = pair.first;
		SHORT state = GetAsyncKeyState(buttonID);

		//�O��̏�Ԃ�ۑ�
		(*(m_OldButtonState.find(buttonID))).second =
			(*(m_NowButtonState.find(buttonID))).second;

		//���̏�Ԃ��X�V
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
	//�O��̏�Ԃ�ۑ�
	m_OldPadState = m_NowPadState;

	//���̏�Ԃ��X�V
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

