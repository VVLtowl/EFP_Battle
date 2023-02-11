#pragma once

#include <Windows.h>
#include <Xinput.h>
#include <unordered_map>

#pragma comment(lib,"xinput.lib")

#define FIRSTTOOL_API __declspec(dllexport)

/*********************************************************
* @brief		���͐���
* @details		�E�L�[�{�[�h
*					������
*					�X�V����
*					�I������
*					�{�^��������Ă����Ԃ̊m�F
*					�{�^�������ꂽ�u�Ԃ̊m�F
*					�{�^�������ꂽ�u�Ԃ̊m�F
*				�E�}�E�X
*					������
*					�X�V����
*					�I������
*					�{�^��������Ă����Ԃ̊m�F
*					�{�^�������ꂽ�u�Ԃ̊m�F
*					�{�^�������ꂽ�u�Ԃ̊m�F
*					�}�E�X���W�̎擾
*				�E�Q�[���p�b�g
*					������
*					�X�V����
* 					�I������
*					�{�^��������Ă����Ԃ̊m�F
* 					�{�^�������ꂽ�u�Ԃ̊m�F
* 					�{�^�������ꂽ�u�Ԃ̊m�F
*					�g���K�[������Ă����Ԃ̊m�F
* 					�g���K�[�����ꂽ�u�Ԃ̊m�F
* 					�g���K�[�����ꂽ�u�Ԃ̊m�F
*					�X�e�B�b�N�|����W�̎擾
*					�o�C�u���[�V�����̐ݒ�
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

// @brief		�L�[�{�[�h���͑Ώ�
class KeyboardInput
{
public:
	// @brief	������
	void Init();

	// @brief	�I������
	void Uninit();

	// @brief	�X�V����
	void Update();

	/*********************************************************
	* @brief	�{�^��������Ă����Ԃ̊m�F
	* @param	[in]keyID: �L�[��ID�@��F'A'
	********************************************************/
	bool IsKeyPress(BYTE keyID);

	/*********************************************************
	* @brief	�{�^�������ꂽ�u�Ԃ̊m�F
	* @param	[in]keyID: �L�[��ID�@��F'A'
	********************************************************/
	bool IsKeyDown(BYTE keyID);

	/*********************************************************
	* @brief	�{�^�������ꂽ�u�Ԃ̊m�F
	* @param	[in]keyID: �L�[��ID�@��F'A'
	********************************************************/
	bool IsKeyUp(BYTE keyID);

private:
	// @brief	���O�̃t���[���ł̃L�[��Ԃ��i�[�p
	BYTE m_OldKeyState[NUM_KEY_MAX];

	// @brief	���݂̃t���[���ł̃L�[��Ԃ��i�[�p
	BYTE m_NowKeyState[NUM_KEY_MAX];
};

#pragma endregion

#pragma region ========== mouse ==========
//========== mouse button macro ==========
const int MOUSE_LEFT = VK_LBUTTON;
const int MOUSE_RIGHT = VK_RBUTTON;
const int MOUSE_MID = VK_MBUTTON;

//========== mouse input class definition ==========

// @brief		�}�E�X���͑Ώ�
class MouseInput
{
public:
	// @brief	������
	void Init(HWND wnd);

	// @brief	�I������
	void Uninit();

	// @brief	�X�V����
	void Update();

	/*********************************************************
	* @brief	�{�^��������Ă����Ԃ̊m�F
	* @param	[in]buttonID: �{�^����ID�@��FMOUSE_LEFT
	********************************************************/
	bool IsButtonPress(int buttonID);

	/*********************************************************
	* @brief	�{�^�������ꂽ�u�Ԃ̊m�F
	* @param	[in]buttonID: �{�^����ID�@
	********************************************************/
	bool IsButtonDown(int buttonID);

	/*********************************************************
	* @brief	�{�^�������ꂽ�u�Ԃ̊m�F
	* @param	[in]buttonID: �{�^����ID�@
	********************************************************/
	bool IsButtonUp(int buttonID);

	/*********************************************************
	* @brief	�}�E�X���W�̎擾
	********************************************************/
	POINT GetMousePos();

	/*********************************************************
	* @brief	�}�E�X�E�B���h�E�Y���W�̎擾
	********************************************************/
	POINT GetMouseWinPos();

private:
	// @brief	���O�̃t���[���ł̃{�^����Ԃ��i�[�p
	std::unordered_map<int, SHORT> m_OldButtonState;

	// @brief	���݂̃t���[���ł̃{�^����Ԃ��i�[�p
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

// @brief		�X�e�B�b�N�|��l�̍ő�l(-32728~32727)
const unsigned short STICK_MOVE_MAX = 32728;

// @brief		�o�C�u���[�V�����̍ő�l(0~65535)
const unsigned short VIBRATION_MAX = 65535;

//========== gamepad input class definition ==========

// @brief		�}�E�X���͑Ώ�
class GamePadInput
{
public:
	/*********************************************************
	* @brief	������
	* @param	[in]userID: ���[�U�[ID��ݒ�i���̃p�b�h���LID�j
	********************************************************/
	void Init(int userID);

	// @brief	�I������
	void Uninit();

	// @brief	�X�V����
	void Update();

	/*********************************************************
	* @brief	�{�^��������Ă����Ԃ̊m�F
	* @param	[in]buttonID: �{�^����ID�@��FXPAD_LEFT
	********************************************************/
	bool IsButtonPress(DWORD buttonID);

	/*********************************************************
	* @brief	�{�^�������ꂽ�u�Ԃ̊m�F
	* @param	[in]buttonID: �{�^����ID�@
	********************************************************/
	bool IsButtonDown(DWORD buttonID);

	/*********************************************************
	* @brief	�{�^�������ꂽ�u�Ԃ̊m�F
	* @param	[in]buttonID: �{�^����ID�@
	********************************************************/
	bool IsButtonUp(DWORD buttonID);

	/*********************************************************
	* @brief	�g���K�[������Ă����Ԃ̊m�F
	* @return	�g���K�[��]�p�x�̊���
	********************************************************/
	float GetLeftTriggerPress();
	float GetRightTriggerPress();

	/*********************************************************
	* @brief	�g���K�[�����ꂽ�u�Ԃ̊m�F
	********************************************************/
	bool IsLeftTriggerDown();
	bool IsRightTriggerDown();

	/*********************************************************
	* @brief	�g���K�[�����ꂽ�u�Ԃ̊m�F
	********************************************************/
	bool IsLeftTriggerUp();
	bool IsRightTriggerUp();

	/*********************************************************
	* @brief	�X�e�B�b�N�|����W�̎擾
	* @return	�X�e�B�b�N�|����Wx��y (-32768 ���� 32767)
	********************************************************/
	POINT GetLeftStickPos();
	POINT GetRightStickPos();

	/*********************************************************
	* @brief	�o�C�u���[�V�����̐ݒ�
	* @param	[in]leftSpeed: ���o�C�u���[�V�����̋��x (0 ���� 65535)
	* @param	[in]rightSpeed: �E�o�C�u���[�V�����̋��x (0 ���� 65535)
	********************************************************/
	void SetVibration(unsigned short leftSpeed, unsigned short rightSpeed);

private:
	// @brief		�O��̃p�b�g��Ԋi�[�p
	_XINPUT_STATE m_OldPadState;

	// @brief		���݂̃p�b�g��Ԋi�[�p
	_XINPUT_STATE m_NowPadState;

	// @brief		�o�C�u���[�V�����̏��
	_XINPUT_VIBRATION m_Vibration;

	// @brief		���[�U�[ID
	int m_UserID{ 0 };

public:
	// @brief		�ӊO�폜�̏ꍇ�A�f�X�g���N�^�ŏI������
	~GamePadInput();
};

#pragma endregion



