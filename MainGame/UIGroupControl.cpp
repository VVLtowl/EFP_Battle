#include "main.h"
#include "UIGroupControl.h"

UIGroupControl::UIGroupControl(GameObject* owner):
	Component(owner, COMP_UIGROUPCTRL)
{
	UISystem::Register(this);
}

UIGroupControl::~UIGroupControl()
{
	UISystem::Quit(this);
}
