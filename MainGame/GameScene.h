#pragma once
#include "SceneManager.h"

class GameScene :
    public Scene
{
public:
    void MainLoad()override;
    void MainUnload()override;
    void MainUpdate()override;
};

class ServerGameScene :
    public Scene
{
public:
    void MainLoad()override;
    void MainUnload()override;
    void MainUpdate()override;
};

