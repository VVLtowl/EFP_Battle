#pragma once
#include "Component.h"
#include <string>
#include <functional>

class Button :
    public Component
{
    /*********************************************************
    * @brief    component override
    ********************************************************/
public:
    Button(class GameObject* owner, 
        int order = COMP_BUTTON);
    void Update()override;

    /*********************************************************
    * @brief    button event
    ********************************************************/
public:
    void AddDisableEvent(std::function<void()> onDisable);
    void AddEnableEvent(std::function<void()> onEnable);
    void AddClickEvent(std::function<void()> onClick);
    void AddEnterEvent(std::function<void()> onEnter);
    void AddExitEvent(std::function<void()> onExit);
private:
    void OnDisable();
    void OnEnable();
    void OnClick();
    void OnEnter();
    void OnExit();
private:
    std::list<std::function<void()>> m_OnDisables;
    std::list<std::function<void()>> m_OnEnaables;
    std::list<std::function<void()>> m_OnClicks;
    std::list<std::function<void()>> m_OnEnters;
    std::list<std::function<void()>> m_OnExits;

    /*********************************************************
    * @brief    button state
    ********************************************************/
public:
    enum class State
    {
        IDLE,
        HIGHLIGHTED,
        CLICKED,
        DISABLE,
    };
public:
    virtual void ChangeState(State state);
    State& GetButtonState() const { return m_State; };
private:
    State m_State;
};

