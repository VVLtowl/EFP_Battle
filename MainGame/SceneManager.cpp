#include "main.h"
#include "SceneManager.h"

#include "GameManager.h"
#include "MyNetManager.h"


/*********************************************************
* @brief	SceneManager
********************************************************/

Scene* SceneManager::m_CurrentScene;
Scene* SceneManager::m_NextScene;

void SceneManager::Init()
{
}

void SceneManager::Update()
{
	if (m_CurrentScene)
	{
		m_CurrentScene->Update();
	}
}

void SceneManager::Uninit()
{
	UnloadCurrentScene();
}

void SceneManager::CheckChange()
{
	if (m_NextScene)
	{
		UnloadCurrentScene();
		m_CurrentScene = m_NextScene;
		m_CurrentScene->Load();
		m_NextScene = nullptr;
	}
}

void SceneManager::UnloadCurrentScene()
{
	if (m_CurrentScene != nullptr)
	{
		m_CurrentScene->Unload();
		delete m_CurrentScene;
	}
}









/*********************************************************
* @brief	Scene
********************************************************/

void Scene::Load()
{
	CreateDefaultMainCamera();
	CreateDefaultLight();

	MainLoad();
}

void Scene::Unload()
{
	MainUnload();

	ClearGameObjects();
}

void Scene::Update()
{
	MainUpdate();

	UpdateGameExecutors();

	UpdateNetwork();

	UpdateGameObjects();

	UpdateDraw();
}







void Scene::CreateDefaultMainCamera()
{
	GameObjectManager::Create<MainCamera>();
}

void Scene::CreateDefaultLight()
{
	GameObjectManager::Create<MainLight>();
}

void Scene::UpdateGameObjects()
{
	//update gameobject
	GameObjectManager::Update();
}

void Scene::UpdateGameExecutors()
{
	//logic update
	GameManager::UpdateExecutors();
}

void Scene::UpdateNetwork()
{
	//network 
	MyNetManager::Instance()->Update();
}

void Scene::UpdateDraw()
{

#if 0
	//test render tex
	Renderer::BeginTest();
	LightManager::SelectMainLightView();
	DrawManager::DrawAll();
#endif

	//depth view draw
	{
		//shadow depth draw
		Renderer::BeginDepth();
		Renderer::SetLight(
			LightManager::GetMainLight()->LightData);

		//ライトカメラの行列をセット
		LightManager::SelectMainLightView();
		//CameraManager::SelectMainCameraView();

		//影を落としたいオブジェクトを描画(一応地面も)
		DrawManager::DrawShadowDepth();
	}

	//free camera draw
	{
		CameraManager::UpdateCameraDraw();
	}


	//main camera draw
	{
		//prepare for draw
		Renderer::Begin();

		Renderer::SetCamera(
			CameraManager::GetMainCamera()->m_CameraData);

		//メインカメラの行列をセット
		CameraManager::SelectMainCameraView();

		//draw game view
		DrawManager::DrawAll();
	}
	
	//test reset instance count
	Polygon2DMany::Instance()->EndFrame();
}

void Scene::ClearGameObjects()
{
	GameObjectManager::Clear();
}
