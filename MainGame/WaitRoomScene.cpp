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
		//test particle system
		//todo
		if (1)
		{
			ParticleSystem* ps = GameObjectManager::Create<ParticleSystem>();
			ps->GetTransform()->SetPosition(0, 0, 0);
			ps->GetTransform()->SetScale(1, 1, 1);
			MoveControl* move = new MoveControl(ps);
			move->TargetTransform = ps->GetShooter();
		}

		//test cmr plane	
		if (1)
		{
			Plane* plane = GameObjectManager::Create<Plane>();
			plane->GetTransform()->SetRotation(PI / 2, 0, 0);
			plane->GetTransform()->SetPosition(0, 0, 0);
			plane->GetTransform()->SetScale(2, 2, 1);

			Sphere* sky = GameObjectManager::Create<Sphere>();

			//test set main camera look at plane pos
			CameraManager::GetMainCamera()->CmrLookAt->TargetTransform = sky->GetTransform();
			//test set main light look at plane pos
			LightManager::GetMainLight()->LightLookAt->TargetTransform = sky->GetTransform();


			//FreeCamera* cmr = GameObjectManager::Create<FreeCamera>();
			//cmr->GetTransform()->SetPosition(0, 0, -0.5f);
			//cmr->GetTransform()->SetParent(plane->GetTransform());

			//plane->Image->Texture = cmr->GetCamera()->MainSRV;
			//plane->Image->Texcoord = { 1,0,-1,1 };
			//cmr->GetCamera()->CmrLookAt->TargetTransform = lookAtTrs->GetTransform();
		}

		//test sky
		if (0)
		{
			//sky->SetName("Sky");
		}

		//test cube
		if (1)
		{
			Cube* cube = GameObjectManager::Create<Cube>();
			cube->GetTransform()->SetPosition(0, 2, 0);
			cube->SetName("Cube");
			MoveControl* move = new MoveControl(cube);
		}


	}


	//start client 
	Client* client = Client::Instance();
	{
		//logic entrance
		client->StartBH(client->BH_SelectServer);

		//test
		DebugInfo::TestBlocks.emplace(TESTBLOCKID_CLIENT_WAITROOM, [client]()
			{
				ImGui::Begin("Wait Room");

				if (client->JoinSuccess)
				{
					//show waiting clients num
					ImGui::Text(
						"players: %d / %d",
						client->JoinedClientNum,
						client->TargetClientNum);

				}
				else
				{
					//hint to connect
					ImGui::Text("please join a room!");
				}

				ImGui::End();
			});
	}
}

void ClientWaitScene::MainUnload()
{
	DebugInfo::TestBlocks.erase(TESTBLOCKID_CLIENT_WAITROOM);
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
	Server* server = Server::Instance();
	{
		//logic entrance
		server->StartBH(server->BH_InputGameRoom);
	}
}

void ServerSettingScene::MainUnload()
{
	//DebugInfo::TestBlocks.erase(TESTBLOCKID_SERVER_WAITCLIENTINROOM);
}

void ServerSettingScene::MainUpdate()
{
}
#pragma endregion
