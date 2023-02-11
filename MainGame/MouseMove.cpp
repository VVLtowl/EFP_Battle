#include "main.h"
#include "MouseMove.h"

MouseMove::MouseMove()
{
	m_LastPos = Input::Mouse.GetMouseWinPos();
}

MouseMove::~MouseMove()
{
}

void MouseMove::UpdateGameObject()
{
	POINT pt= Input::Mouse.GetMouseWinPos();
	m_Speed = D3DXVECTOR2(pt.x - m_LastPos.x, pt.y - m_LastPos.y);
	m_Speed.x /= SCREEN_WIDTH;
	m_Speed.y /= SCREEN_HEIGHT;
	m_LastPos = pt;
}
