#include "main.h"
#include "WaitRoomScene.h"

#include "imgui/imgui.h"

#include "ParticleSystem.h"
#include "MoveControl.h"
#include "LookAt.h"

#include "Judgement.h"
#include "Client.h"
#include "Server.h"

#include "ClientBehaviour.h"
#include "ServerBehaviour.h"
#include "MyNetManager.h"

#pragma region ========== client wait scene ==========
void ClientWaitScene::MainLoad()
{
	//test obj
	{
		//test cmr plane	
		if (1)
		{
			Plane* plane = GameObjectManager::Create<Plane>();
			plane->GetTransform()->SetRotation(PI / 2, 0, 0);
			plane->GetTransform()->SetPosition(0,1000, 0);
			plane->GetTransform()->SetScale(2, 2, 1);

			Sphere* sky = GameObjectManager::Create<Sphere>();

			//test set main camera look at plane pos
			CameraManager::GetMainCamera()->m_CmrLookAt->m_TargetTransform = sky->GetTransform();
			//test set main light look at plane pos
			LightManager::GetMainLight()->LightLookAt->m_TargetTransform = sky->GetTransform();
		}

		//test particle system
		//todo
		//need repair
		if (1)
		{
			ParticleSystem* ps = GameObjectManager::Create<ParticleSystem>();
			ps->GetTransform()->SetPosition(0, 0, 0);
			ps->GetTransform()->SetScale(1, 1, 1);
			MoveControl* move = new MoveControl(ps);
			move->TargetTransform = ps->GetShooter();
		}

		//test cube
		if (0)
		{
			Cube* cube = GameObjectManager::Create<Cube>();
			cube->GetTransform()->SetPosition(0, 2, 0);
			cube->SetName("Cube");
			MoveControl* move = new MoveControl(cube);
		}
	}

	//clean game manager player and piece
	GameManager::ClearPlayerAndPieces();

	//logic entrance
	AppClient::Instance()->StartInWaitRoomScene();
}

void ClientWaitScene::MainUnload()
{
}

void ClientWaitScene::MainUpdate()
{
	//test scene change
	if (Input::Mouse.IsButtonDown(MOUSE_MID))
	{
		//test
		return;

		SceneManager::ChangeScene<TestScene>();
		AppClient* client = AppClient::Instance();
		{
			client->CheckConnect();

			client->Uninit();
			//MyNetManager::Instance()->Close();
		}
		return;
	}
}
#pragma endregion

#pragma region ========== server setting scene ==========
void ServerSettingScene::MainLoad()
{
	//clean game manager
	GameManager::ClearPlayerAndPieces();

	//judgement reset
	Judgement::Instance()->m_Mode.Clean();
	Judgement::Instance()->m_Mode.SetBadGoodTargetNum(4, 2);

	//logic entrance
	AppServer::Instance()->StartInWaitRoomScene();
}

void ServerSettingScene::MainUnload()
{
}

void ServerSettingScene::MainUpdate()
{
}
#pragma endregion
