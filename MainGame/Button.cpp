#include "main.h"
#include "Button.h"

Button::Button(GameObject* owner, int order):
Component(owner,order),
m_State(State::IDLE)
{
}

void Button::Update()
{
}

void Button::ChangeState(State next)
{
	if (next == m_State)
	{
		return;
	}


	if (m_State != State::DISABLE && next == State::DISABLE)
	{
		OnDisable();
		m_State = next;
		return;
	}
	if (m_State == State::DISABLE && next == State::IDLE)
	{
		OnEnable();
		m_State = next;
		return;
	}

	if (m_State == State::IDLE && next == State::HIGHLIGHTED)
	{
		OnEnter();
		m_State = next;
		return;
	}
	
	if (m_State == State::HIGHLIGHTED && next == State::IDLE)
	{
		OnExit();
		m_State = next; 
		return;
	}

	if (m_State == State::HIGHLIGHTED && next == State::CLICKED)
	{
		OnClick();
		m_State = next;
		return;
	}
}

void Button::OnDisable()
{
	for (auto func : m_OnDisables)
	{
		func();
	}
}

void Button::OnEnable()
{
	for (auto func : m_OnEnaables)
	{
		func();
	}
}

void Button::OnClick()
{
	for (auto func: m_OnClicks)
	{
		func();
	}
}

void Button::OnEnter()
{
	for (auto func : m_OnEnters)
	{
		func();
	}
}

void Button::OnExit()
{
	for (auto func : m_OnExits)
	{
		func();
	}
}

void Button::AddDisableEvent(std::function<void()> onDisable)
{
	m_OnDisables.emplace_back(onDisable);
}

void Button::AddEnableEvent(std::function<void()> onEnable)
{
	m_OnEnaables.emplace_back(onEnable);
}

void Button::AddClickEvent(std::function<void()> onClick)
{
	m_OnClicks.emplace_back(onClick);
}

void Button::AddEnterEvent(std::function<void()> onEnter)
{
	m_OnEnters.emplace_back(onEnter);
}

void Button::AddExitEvent(std::function<void()> onExit)
{
	m_OnExits.emplace_back(onExit);
}

