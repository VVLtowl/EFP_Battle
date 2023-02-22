#include "main.h"

#include "resource.h"
#include "SceneManager.h"
//#include "drawManager.h"
//#include "cameraManager.h"
//#include "lightManager.h"
//#include "transformManager.h"
//#include "gameObjectManager.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#include <iostream>
#include <time.h>
#include <string>
#include <string.h>

#include "LookAt.h"
#include "LookAtCamera.h"
#include "TransformAnime.h"
#include "WorldToScreen.h"
#include "MoveControl.h"
#include "BoxCollider.h"
#include "GameManager.h"

#include "NetworkManager.h"

#pragma region ========== prototype declaration ==========
/*********************************************************
* @brief	ウィンドウズプログラムに必要な関数
********************************************************/
// @detail	メッセージ処理でimguiのプロセス更新も行う
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

/*********************************************************
* @brief	imguiに必要な関数：imguiのプロセス更新
********************************************************/
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/*********************************************************
* @brief	メインサイクル
********************************************************/
void Init();
void Update();
void Uninit();
#pragma endregion

#pragma region ========== window ==========
/*********************************************************
* @brief	ウィンドウズインスタンス
********************************************************/
HWND g_Window;
DWORD g_FrameRate;

/*********************************************************
* @brief	ウィンドウ関数定義
********************************************************/
char* g_CaptureName;
char g_SubName[64] = "";
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#pragma region ========== create window ==========

		WNDCLASSEX wcex =
		{
			sizeof(WNDCLASSEX),
			CS_CLASSDC,
			WndProc,
			0,
			0,
			hInstance,
			NULL,
			LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)(COLOR_WINDOW + 1),
			NULL,
			CLASS_NAME,
			NULL
		};

		RegisterClassEx(&wcex);
		RECT rc = { 0,0,(LONG)SCREEN_WIDTH ,(LONG)SCREEN_HEIGHT };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		g_CaptureName = (char*)malloc(256);
		memset(g_CaptureName, 0, 64);
		strcpy(g_CaptureName, WINDOW_NAME);
		strcat(g_CaptureName, g_SubName);

		g_Window = CreateWindowEx(0,
			CLASS_NAME,
			g_CaptureName,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rc.right-rc.left,
			//(SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2),
			rc.bottom-rc.top,
			//(SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION)),
			NULL,
			NULL,
			hInstance,
			NULL);


		ShowWindow(g_Window, nCmdShow);
		UpdateWindow(g_Window);
#pragma endregion

#pragma region ========== main cycle ==========
		//main init 
		Init();

		//time init
		DWORD dwExecLastTime;
		DWORD dwCurrentTime;
		DWORD dwFPSLastTime;
		DWORD dwFrameCount;
		timeBeginPeriod(1);
		dwFPSLastTime = dwExecLastTime = timeGetTime();
		dwCurrentTime = 0;
		dwFrameCount = 0;

		//create msg instance
		MSG msg;
		try
		{
			while (1)
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					if (msg.message == WM_QUIT)
					{
						throw std::exception("normal test exception");
						//make log

						//DebugInfo::WriteInfoToLog("..\\MainGame\\TestLog.txt");
						if (NetworkManager::Instance()->TargetClient)
						{
							DebugInfo::WriteInfoToLog("..\\MainGame\\ClientLog.txt");
						}
						if (NetworkManager::Instance()->TargetServer)
						{
							DebugInfo::WriteInfoToLog("..\\MainGame\\ServerLog.txt");
						}
						break;
					}
					else
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
				else
				{
					dwCurrentTime = timeGetTime();

					if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
					{
						dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
						g_FrameRate = dwFrameCount;
						dwFrameCount = 0;							// カウントをクリア
					}

					if ((dwCurrentTime - dwExecLastTime) >= (1000 / FPS))
					{
						//time update
						{
							dwFrameCount++;
							dwExecLastTime = dwCurrentTime;
						}

						//update
						Update();
					}

				}
			}
		}
		catch (const std::exception& m)
		{
			DebugInfo::Print("exception：" + std::string(m.what()));

			//make log
			if (NetworkManager::Instance()->TargetClient)
			{
				DebugInfo::WriteInfoToLog("..\\MainGame\\ClientLog.txt");
			}
			if (NetworkManager::Instance()->TargetServer)
			{
				DebugInfo::WriteInfoToLog("..\\MainGame\\ServerLog.txt");
			}
		}

		//uninit
		Uninit();

		timeEndPeriod(1);
#pragma endregion

		

	UnregisterClass(CLASS_NAME, wcex.hInstance);

	return (int)msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//network
	NetworkManager::Instance()->UpdateNetProc(uMsg,lParam);

	//imgui
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
	{
		return true;
	}

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
void SetSubName(const char* sub)
{
	sprintf(g_SubName, "%s", sub);
	SetWindowText(GetWindow(), g_SubName);
}
HWND GetWindow()
{
	return g_Window;
}
DWORD GetFrameRate()
{
	return g_FrameRate;
}
#pragma endregion






#pragma region ========== main cycle ==========
/*********************************************************
* @brief	メインサイクル関数定義
********************************************************/
void Init()
{
	//other 
	srand((unsigned int)time(0));

	//renderer init
	Renderer::Init();

	//input init
	Input::Init();

	//flyweight pattern
	DrawManager::Load();

	//imgui init
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(g_Window);
		ImGui_ImplDX11_Init(Renderer::GetDevice(), Renderer::GetDeviceContext());
	}

	//game manager init
	GameManager::Init();

	//scene init
	SceneManager::Init();

	//first scene
	//SceneManager::Instance()->ChangeScene<TitleScene>();
	SceneManager::ChangeScene<TestScene>();
}
void Update()
{
	
		//input update
		Input::Update();

		//scene update
		SceneManager::Update();

		//draw imgui
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			DebugInfo::Update();


			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}

		//draw over
		Renderer::End(true);

		//scene change
		SceneManager::CheckChange();

}
void Uninit() 
{
	GameManager::Uninit();

	//scene unload
	SceneManager::Uninit();

	DrawManager::Unload();
	TextureFactory::Clear();
	ModelFactory::Clear();


	//imgui uninit
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//renderer uninit
	Renderer::Uninit();

	//input uninit
	Input::Uninit();
}
#pragma endregion