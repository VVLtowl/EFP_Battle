#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <assert.h>

#pragma warning(push)
#pragma warning(disable:4005)
#include <d3d11.h>
#include <d3dx9.h>
#include <d3dx11.h>
#pragma warning(pop)

#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "dxgi.lib")

#include "input/Input.h"
//#pragma comment (lib, "IInput.lib")


#include "Math.h"
#include "Collision.h"
#include "debugInfo.h"
#include "renderer.h"
#include "resource.h"

#include "drawManager.h"
#include "colliderManager.h"
#include "lightManager.h"
#include "cameraManager.h"
#include "inputManager.h"
#include "uiSystem.h"
#include "gameObjectManager.h"



#pragma region ========== common macro definition ==========
/*********************************************************
* @brief	ウィンドウサイズ情報
********************************************************/
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 540;
const float SCREEN_WIDTH_HALF = 480;
const float SCREEN_HEIGHT_HALF = 270;
const char* const CLASS_NAME = "AppClass";
const char* const WINDOW_NAME = "EFP";

/*********************************************************
* @brief	時間系
********************************************************/
const unsigned int FPS = 60;
const float FRAME_STRIDE = 1.0f;

#pragma endregion
/*********************************************************
* @brief
********************************************************/
void SetWinSubName(const char* sub);

/*********************************************************
* @brief	ウィンドウのハンドルを取得
********************************************************/
HWND GetWindow();

/*********************************************************
* @brief	FPSを取得
********************************************************/
DWORD GetFrameRate();

