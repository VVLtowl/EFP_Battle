#include "main.h"
#include "inputManager.h"

MouseInput Input::Mouse;
KeyboardInput Input::Keyboard;

void Input::Init()
{
	Mouse.Init(GetWindow());
	Keyboard.Init();
}

void Input::Update()
{
	Mouse.Update();
	Keyboard.Update();
}

void Input::Uninit()
{
	Mouse.Uninit();
	Keyboard.Uninit();
}

