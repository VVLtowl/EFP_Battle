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
#include "NetworkManager.h"

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


			//FreeCamera* cmr = GameObjectManager::Create<FreeCamera>();
			//cmr->GetTransform()->SetPosition(0, 0, -0.5f);
			//cmr->GetTransform()->SetParent(plane->GetTransform());

			//plane->Image->Texture = cmr->GetCamera()->MainSRV;
			//plane->Image->Texcoord = { 1,0,-1,1 };
			//cmr->GetCamera()->CmrLookAt->TargetTransform = lookAtTrs->GetTransform();
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

	//logic entrance
	Client::Instance()->StartInWaitRoomScene();
}

void ClientWaitScene::MainUnload()
{
}

void ClientWaitScene::MainUpdate()
{
	//test scene change
	if (Input::Mouse.IsButtonDown(MOUSE_MID))
	{
		SceneManager::ChangeScene<TestScene>();
		Client* client = Client::Instance();
		{
			client->StartBH(client->BH_Uninit);
			NetworkManager::Instance()->Close();
		}
		return;
	}
}
#pragma endregion

#pragma region ========== server setting scene ==========
void ServerSettingScene::MainLoad()
{
	//logic entrance
	Server::Instance()->StartInWaitRoomScene();
}

void ServerSettingScene::MainUnload()
{
	//DebugInfo::TestBlocks.erase(TESTBLOCKID_SERVER_WAITCLIENTINROOM);
}

void ServerSettingScene::MainUpdate()
{
}
#pragma endregion
