#include "main.h"
#include "BehaviourFactory.h"
#include "GameManager.h"
#include "GameScene.h"

#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "Judgement.h"
#include "Client.h"
#include "Board.h"

#include "LookAt.h"
#include "TransformAnime.h"
#include "WorldToScreen.h"

#include "NetworkManager.h"

void GameScene::MainLoad()
{
	//test game scene load object
	if (0)
	{
		//test plane
		Plane* plane = GameObjectManager::Create<Plane>();
		plane->GetTransform()->SetRotation(PI / 2.0f, 0, 0);
		plane->GetTransform()->SetPosition(0, 0, 0);

		//test set main camera look at cubeCenter pos
		CameraManager::GetMainCamera()->m_CmrLookAt->m_TargetTransform = plane->GetTransform();

		//test set main light look at cubeCenter pos
		LightManager::GetMainLight()->LightLookAt->m_TargetTransform = plane->GetTransform();

		//test light
		//MainLight* lit = GameObjectManager::Create<MainLight>();

		////test camera
		//MainCamera* cmr = GameObjectManager::Create<MainCamera>();
		////cmr->GetCamera()->LookAt = plane->GetTransform();

		//test obj
		if (0)
		{
			Torus* t = GameObjectManager::Create<Torus>();
			Torus* t2 = GameObjectManager::Create<Torus>();
			Torus* t3 = GameObjectManager::Create<Torus>();

			t2->GetTransform()->SetParent(t->GetTransform());
			t2->GetTransform()->SetPosition(1, 1, 0);
			t2->GetTransform()->SetScale(0.5f, 0.5f, 0.5f);

			t3->GetTransform()->SetParent(t2->GetTransform());
			t3->GetTransform()->SetPosition(0, 1, 0);
			t3->GetTransform()->SetScale(0.5f, 0.5f, 0.5f);

			//test line painter
			DrawLineDescription dldesc;
			dldesc.StartTransform = t3->GetTransform();
			dldesc.EndTransform = t->GetTransform();
			dldesc.Color = { 5,2,2,1 };
			dldesc.Size = 0.01f;
			LinePainter::Instance()->MakeLine(dldesc);

			//test transform anime
			if (0)
			{
				TransformAnimeDescripition tadesc(t->GetTransform(), t->GetTransform());
				tadesc.LoopCount = INT_MAX;
				tadesc.EndPosition = { 0,3,0 };
				D3DXQuaternionRotationYawPitchRoll(&tadesc.EndRotation, PI / 3, PI / 2, PI / 4);
				tadesc.EndScale = V3_ONE * 0.5f;
				new TransformAnime(t, tadesc);
			}
		}

		//test depth UI for shadow map
		if (0)
		{
			DepthUI* depthUI = GameObjectManager::Create<DepthUI>();
		}

		//test cube and collision
		if (0)
		{
			Cube* cb = GameObjectManager::Create<Cube>();
			cb->GetTransform()->SetPosition(0, 3, 2);
			cb->GetTransform()->SetRotation(PI / 3, PI / 3, 0);
			cb->GetTransform()->SetScale(1, 1, 0.5f);
		}

		//test ActSelection button
		if (0)
		{
			D3DXVECTOR3 actSelectPos = { -3,2,-0.5f };
			D3DXVECTOR3 actSelectScl = V3_ONE;
			ActSelection* actSelect = new ActSelection(actSelectPos, actSelectScl);
		}

		//test particle system
		if (0)
		{
			ParticleSystem* ps = GameObjectManager::Create<ParticleSystem>();
			ps->GetTransform()->SetPosition(0, 0, 0);
			ParticleSystem* ps2 = GameObjectManager::Create<ParticleSystem>();
			ps2->GetTransform()->SetPosition(-2, 2, 0);
			ParticleSystem* ps3 = GameObjectManager::Create<ParticleSystem>();
			ps3->GetTransform()->SetPosition(2, 1, 0);
		}

		//test world to screen
		if (0)
		{
			Cube* cb = GameObjectManager::Create<Cube>();
			cb->GetTransform()->SetPosition(0, 3, 2);
			UIFinishMark* ui = GameObjectManager::Create<UIFinishMark>();
			ui->FollowWorldObject->SetTargetTransform(cb->GetTransform());
		}
	}

	//logic entrance
	Client::Instance()->StartInGameScene();
}

void GameScene::MainUnload()
{
	//uninit game manager
	//GameManager::CleanGameScene();

	//clear behaviours
	BehaviourFactory::Clear();

	//clear data
	Client::Instance()->ClearDataInGameScene();
}

void GameScene::MainUpdate()
{
	//test scene change
	if (Input::Mouse.IsButtonDown(MOUSE_MID))
	{
		//test
		//SceneManager::ChangeScene<TitleScene>();
		return;
	}

	//logic update
	//GameManager::Instance()->UpdateExecutors();
}




void ServerGameScene::MainLoad()
{
	//test plane
	Plane* plane = GameObjectManager::Create<Plane>();
	plane->GetTransform()->SetRotation(PI / 2.0f, 0, 0);
	plane->GetTransform()->SetPosition(0, 0, 0);

	//test set main camera look at cubeCenter pos
	CameraManager::GetMainCamera()->m_CmrLookAt->m_TargetTransform = plane->GetTransform();

	//test set main light look at cubeCenter pos
	LightManager::GetMainLight()->LightLookAt->m_TargetTransform = plane->GetTransform();

	//entrance
	Judgement::Instance()->GameStart();
}

void ServerGameScene::MainUnload()
{
	//exit main game
	Judgement::Instance()->GameEnd();
}

void ServerGameScene::MainUpdate()
{
}
