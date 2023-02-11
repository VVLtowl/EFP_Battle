#include "main.h"
#include "uiSystem.h"

std::list<UIGroupControl*> UISystem::m_UIGroupControls;

void UISystem::Register(UIGroupControl* ugc)
{
	m_UIGroupControls.emplace_back(ugc);
}

void UISystem::Quit(UIGroupControl* ugc)
{
	m_UIGroupControls.remove(ugc);
}

void UISystem::Update()
{
	for (auto ugc : m_UIGroupControls)
	{
		ugc->Update();
	}
}
