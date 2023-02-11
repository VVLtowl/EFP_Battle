#pragma once
#include "Component.h"
#include <functional>
#include "inputManager.h"

class MoveControl :
    public Component
{
public:
    MoveControl(class GameObject* owner,
        std::function<bool()> moveLeft = []() {return Input::Keyboard.IsKeyPress(KEY_A); },
        std::function<bool()> moveRight = []() {return Input::Keyboard.IsKeyPress(KEY_D); },
        std::function<bool()> moveUp = []() {return Input::Keyboard.IsKeyPress(KEY_Q); },
        std::function<bool()> moveDown = []() {return Input::Keyboard.IsKeyPress(KEY_E); },
        std::function<bool()> moveForward = []() {return Input::Keyboard.IsKeyPress(KEY_W); },
        std::function<bool()> moveBack = []() {return Input::Keyboard.IsKeyPress(KEY_S); },

        int updateOrder = COMP_MOVECTRL);
    ~MoveControl();
    void Update()override;

public:
    class Transform3D* TargetTransform;
private:
    float m_Speed;
    std::function<bool()> m_MoveLeft;
    std::function<bool()> m_MoveRight;
    std::function<bool()> m_MoveUp;
    std::function<bool()> m_MoveDown;
    std::function<bool()> m_MoveForward;
    std::function<bool()> m_MoveBack;
};

