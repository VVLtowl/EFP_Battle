#pragma once
#include "Singleton.h"
#include <typeinfo>

class Scene
{
public:
    void Load();
    void Unload();
    void Update();

    /*********************************************************
    * @brief    継承先でカスタマイズ
    ********************************************************/
private:
    virtual void MainLoad() = 0;
    virtual void MainUpdate() = 0;
    virtual void MainUnload() = 0;

    /*********************************************************
    * @brief    シーンの共通操作
    ********************************************************/
protected:
    void CreateDefaultMainCamera();
    void CreateDefaultLight();

    void UpdateGameObjects();
    void UpdateGameExecutors();
    void UpdateNetwork();
    void UpdateDraw();

    void ClearGameObjects();
};

#include "TitleScene.h"
#include "GameScene.h"
#include "TestScene.h"
#include "WaitRoomScene.h"

class SceneManager
{
    /*********************************************************
    * @brief    メインループ用
    ********************************************************/
public:
    static void Init();
    static void Update();
    static void Uninit();
    static void CheckChange();
private:
    static Scene* m_CurrentScene;
    static Scene* m_NextScene;

    /*********************************************************
    * @brief    実行シーンを操作
    ********************************************************/
private:
    static void UnloadCurrentScene();
public:
    template <typename SceneType>
    static void ChangeScene();
};

template<typename SceneType>
inline void SceneManager::ChangeScene()
{
    if (m_CurrentScene)
    {
        if (typeid(*m_CurrentScene) == typeid(SceneType))
        {
            m_NextScene = nullptr;
        }
        else
        {
            m_NextScene = new SceneType();
        }
    }
    else
    {
        m_NextScene = new SceneType();
    }
}
