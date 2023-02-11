#pragma once


class Input
{
public:
	static MouseInput Mouse;
	static KeyboardInput Keyboard;

	static void Init();
	static void Update();
	static void Uninit();
};

