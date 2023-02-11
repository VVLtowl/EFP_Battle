#pragma once
#include "SceneManager.h"

class TestScene :
    public Scene
{
public:
    void MainLoad()override;
    void MainUnload()override;
    void MainUpdate()override;
};

