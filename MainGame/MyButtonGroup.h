



//give up




#pragma once
#include "ButtonGroup.h"
#include "RPS.h"
#include <functional>



//#define HINT_MOUSE_RAY

//component
class ButtonHandGroup :
    public ButtonGroup
{
public:
    ButtonHandGroup(class GameObject* owner);
    void OtherUpdate()override;

public:
    HandType SelectedHandType = HandType::SCISSORS;
    void InitHandButtonsClickEvent();
};

//component
class ButtonSelectActGroup :
    public ButtonGroup
{
public:
    ButtonSelectActGroup(class GameObject* owner);
    void OtherUpdate()override;
};

