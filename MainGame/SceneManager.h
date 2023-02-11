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
    * @brief    �p����ŃJ�X�^�}�C�Y
    ********************************************************/
private:
    virtual void MainLoad() = 0;
    virtual void MainUpdate() = 0;
    virtual void MainUnload() = 0;

    /*********************************************************
    * @brief    �V�[���̋��ʑ���
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
    * @brief    ���C�����[�v�p
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
    * @brief    ���s�V�[���𑀍�
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
