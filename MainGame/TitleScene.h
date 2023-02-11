#pragma once
#include "SceneManager.h"

class TitleScene :
    public Scene
{
public:
    void MainLoad()override;
    void MainUnload()override;
    void MainUpdate()override;
};

