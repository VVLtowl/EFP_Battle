#include "main.h"
#include "TestScene.h"
#include "LookAt.h"
#include "LookAtCamera.h"
#include "MoveControl.h"
#include "Button.h"

#include "Judgement.h"
#include "Server.h"
#include "Client.h"

#include "ServerBehaviour.h"
#include "ClientBehaviour.h"

#include "SceneManager.h"
#include "TransformAnime.h"

void TestScene::MainLoad()
{
	//InitGameObjectManager();

	//test mark anime
	if (1)
	{
		UIMark* ui = new UIMark();
		ui->SetMark(UIMark::MARK_THINK);
		ui->GetTransform()->SetPosition(SCREEN_WIDTH_HALF, SCREEN_HEIGHT_HALF, 0);
	
		UISelect* selectMark = new UISelect();
		selectMark->GetTransform()->SetRotation(-PI / 2, 0, 0);
		selectMark->GetTransform()->SetPosition(0, 2, 0);
		{
			Animator* animator = new Animator(selectMark);
			AniDesc_Vec3Hermite sclDesc;
			{
				sclDesc.LoopCount = INT_MAX;
				sclDesc.Duration = 80;
				sclDesc.StartVec3 = V3_ONE * 2;
				sclDesc.EndVec3 = V3_ONE * 2;
				sclDesc.StartTangent = -V3_ONE;
				sclDesc.EndTangent = -V3_ONE;
			}
			ComputeHermiteVec3 computeFunc;
			new Anime_Scale(animator, sclDesc, computeFunc);
		}


		Emitter* effect = new Emitter();
		{
			Animator* animator = new Animator(effect);
			AniDesc_PositionCircleRotate desc;
			{
				desc.LoopCount = INT_MAX;
				desc.Duration = 80;
				desc.StartRadian = PI/2;
				desc.EndRadian = PI*2.5f;
				desc.Radius = 2;
				desc.Center = { 1,2,0 };
			}
			ComputePositionCircleRotate computeFunc;
			new Anime_Position(animator, desc, computeFunc);
		}
		
	}

	//test cmr plane	
	if (1)
	{
		Plane* plane = GameObjectManager::Create<Plane>(); 
		plane->GetTransform()->SetRotation(PI / 2, 0, 0);
		plane->GetTransform()->SetPosition(0, 0, 0);
		plane->GetTransform()->SetScale(2, 2, 1);

		//test set main camera look at plane pos
		CameraManager::GetMainCamera()->CmrLookAt->TargetTransform = plane->GetTransform();
		//test set main light look at plane pos
		LightManager::GetMainLight()->LightLookAt->TargetTransform = plane->GetTransform();

		TransformObject* lookAtTrs = GameObjectManager::Create<TransformObject>();
		lookAtTrs->GetTransform()->SetPosition(0, 0, -1);
		lookAtTrs->GetTransform()->SetParent(plane->GetTransform());

		FreeCamera* cmr = GameObjectManager::Create<FreeCamera>();
		cmr->GetTransform()->SetPosition(0, 0, -0.5f);
		cmr->GetTransform()->SetParent(plane->GetTransform());

		plane->Image->Texture = cmr->GetCamera()->MainSRV;
		plane->Image->Texcoord = { 1,0,-1,1 };
		cmr->GetCamera()->CmrLookAt->TargetTransform = lookAtTrs->GetTransform();
	}

	//test sky
	if (1)
	{
		Sphere* sky = GameObjectManager::Create<Sphere>(); 
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

	//test particle system
	if (1)
	{
		ParticleSystem* ps = GameObjectManager::Create<ParticleSystem>();
		ps->GetTransform()->SetPosition(0, 0, 0);
		MoveControl* move = new MoveControl(ps);
		move->TargetTransform = ps->GetShooter();
	}

	//test button
	if (1)
	{
		ActSelection* buttonStartServer = new ActSelection();
		ActSelection* buttonStartClient = new ActSelection();

		buttonStartServer->GetTransform()->SetPosition(-2, 2, 0); 
		buttonStartServer->m_Polygon2D->Color =
			buttonStartServer->OriginalColor = { 1,0,0,1 };
		buttonStartServer->m_Button->AddClickEvent([]()
			{
				//judgement work in server side
				Judgement::Instance()->Init();

				//create server
				Server* server=Server::Instance();
				
				//logic entrance
				server->StartBH(server->BH_Init);
			});

		buttonStartClient->GetTransform()->SetPosition(2, 2, 0);	
		buttonStartClient->m_Polygon2D->Color =
			buttonStartClient->OriginalColor = { 0,0,1,1 };
		buttonStartClient->m_Button->AddClickEvent([]()
			{
				//create client
				Client* client = Client::Instance();

				//logic entrance
				client->StartBH(client->BH_Init);
			});
	}
}

void TestScene::MainUnload()
{
}

void TestScene::MainUpdate()
{
}
