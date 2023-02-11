#pragma once
#include <list>

#include "UIGroupControl.h"

#include "ButtonGroup.h"
#include "ActOrderUIGroup.h"

class UISystem
{
private:
	static std::list<class UIGroupControl*> m_UIGroupControls;

public:
	static void Register(class UIGroupControl* ugc);
	static void Quit(class UIGroupControl* ugc);
	static void Update();
};

