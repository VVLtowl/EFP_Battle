#pragma once
#include "SceneManager.h"

class ClientWaitScene :
    public Scene
{
    void MainLoad()override;
    void MainUnload()override;
    void MainUpdate()override;
};

class ServerSettingScene :
    public Scene
{
    void MainLoad()override;
    void MainUnload()override;
    void MainUpdate()override;
};

