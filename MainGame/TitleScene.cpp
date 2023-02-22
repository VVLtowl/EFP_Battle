#include "main.h"
#include "TransformAnime.h"
#include "TitleScene.h"

#include "LookAtCamera.h"
#include "LookAt.h"
//test
ActSelection* actSelect[6];

void TitleScene::MainLoad()
{
	//test cmr plane	
	if (1)
	{
		//mirror cube
		{
			TransformObject* cubeCenter = GameObjectManager::Create<TransformObject>();
			cubeCenter->GetTransform()->SetPosition(0, 0, 0);

			//test set main camera look at cubeCenter pos
			CameraManager::GetMainCamera()->m_CmrLookAt->m_TargetTransform = cubeCenter->GetTransform();

			//test set main light look at cubeCenter pos
			LightManager::GetMainLight()->LightLookAt->m_TargetTransform = cubeCenter->GetTransform();

#pragma region ========== cube planes ==========
			float radius = 2;
			Plane* plane[6];
			plane[0] = GameObjectManager::Create<Plane>();
			plane[0]->GetTransform()->SetRotation(PI / 2.0f, 0, 0);
			plane[0]->GetTransform()->SetPosition(0, radius, 0);
			plane[1] = GameObjectManager::Create<Plane>();
			plane[1]->GetTransform()->SetRotation(-PI / 2.0f, 0, 0);
			plane[1]->GetTransform()->SetPosition(0, -radius, 0);
			plane[2] = GameObjectManager::Create<Plane>();
			plane[2]->GetTransform()->SetRotation(0, PI, 0);
			plane[2]->GetTransform()->SetPosition(0, 0, radius);
			plane[3] = GameObjectManager::Create<Plane>();
			plane[3]->GetTransform()->SetRotation(0, 0, 0);
			plane[3]->GetTransform()->SetPosition(0, 0, -radius);
			plane[4] = GameObjectManager::Create<Plane>();
			plane[4]->GetTransform()->SetRotation(0, -PI/2, 0);
			plane[4]->GetTransform()->SetPosition(radius, 0, 0);
			plane[5] = GameObjectManager::Create<Plane>();
			plane[5]->GetTransform()->SetRotation(0, PI/2, 0);
			plane[5]->GetTransform()->SetPosition(-radius, 0, 0);

			for (int i = 0; i < 6; i++)
			{
				plane[i]->GetTransform()->SetScale(1, 1, 1);
				plane[i]->GetTransform()->SetParent(cubeCenter->GetTransform());

				TransformObject* lookAtTrs = GameObjectManager::Create<TransformObject>();
				lookAtTrs->GetTransform()->SetPosition(0, 0, -1);
				lookAtTrs->GetTransform()->SetParent(plane[i]->GetTransform());

				FreeCamera* cmr = GameObjectManager::Create<FreeCamera>();
				cmr->GetTransform()->SetPosition(0, 0, -0.5f);
				cmr->GetTransform()->SetParent(plane[i]->GetTransform());

				cmr->GetCamera()->m_CmrLookAt->m_TargetTransform = (lookAtTrs->GetTransform());
				plane[i]->Image->Texture = cmr->GetCamera()->m_MainSRV;

				D3DXVECTOR3 actSelectPos = { 0,0,-0.1f };
				D3DXVECTOR3 actSelectScl = V3_ONE * 1.6f;
				actSelect[i] = new ActSelection(actSelectPos, actSelectScl);
				actSelect[i]->GetTransform()->SetParent(plane[i]->GetTransform());
				actSelect[i]->m_LookAtCamera->SetState(Component::PAUSED);
			}
#pragma endregion

			//anime
			{
				Animator* animator = new Animator(cubeCenter);
				ComputeUniformQuaternion computeFunc;
				AniDesc_QuaternionStartEnd desc;
				desc.Start = QUAT_IDENTITY;
				D3DXQuaternionRotationYawPitchRoll(&desc.End, PI, PI * 2, PI / 2);
				desc.Duration = 360;
				desc.LoopCount = INT_MAX;
				new Anime_Rotation(animator, desc, computeFunc);
			}
		}
	}

	//test sky
	if (1)
	{
		Sphere* sky = GameObjectManager::Create<Sphere>();
	}

	//test depth UI for shadow map
	if (1)
	{
		DepthUI* depthUI = GameObjectManager::Create<DepthUI>();
	}

	//test cube and collision
	if (1)
	{
		//many
		if (1)
		{
			for (int i = 0; i < 100; i++)
			{
				Cube* cb = GameObjectManager::Create<Cube>();
				cb->GetTransform()->SetRotation(rand() % 100 / 100.0f * PI / 3, rand() % 100 / 100.0f * PI / 3, 0);
				cb->GetTransform()->SetScale(rand() % 100 / 100.0f * 0.2f, rand() % 100 / 100.0f, rand() % 100 / 100.0f * 0.5f);

				Animator* animator = new Animator(cb);
				{
					ComputeUniformQuaternion computeFunc;
					AniDesc_QuaternionStartEnd desc;
					desc.Start = QUAT_IDENTITY;
					D3DXQuaternionRotationYawPitchRoll(&desc.End, PI, PI * 2, PI / 2);
					desc.Duration = 360;
					desc.LoopCount = INT_MAX;
					//new Anime_Rotation(animator, desc, computeFunc);
				}
				{
					ComputeUniformVec3 computeFunc;
					AniDesc_Vec3StartEnd desc;
					desc.Start = {
						(rand() % 100 / 100.0f - 0.5f) * 16,
						10 + rand() % 100 / 100.0f * 10 + i,
						(rand() % 100 / 100.0f - 0.5f) * 16
					};
					desc.End = {
						desc.Start.x,
						desc.Start.y - 60,
						desc.Start.z
					};
					desc.Duration = 720;
					desc.LoopCount = INT_MAX;
					new Anime_Position(animator, desc, computeFunc);
				}
				{
					ComputePositionCircleRotate computeFunc;
					AniDesc_PositionCircleRotate desc;
					desc.Center = V3_ZERO +
						D3DXVECTOR3(
							(rand() % 100 / 100.0f - 0.5f) * 5,
							(rand() % 100 / 100.0f - 0.5f) * 4,
							(rand() % 100 / 100.0f - 0.5f) * 3);
					desc.StartRadian = rand() % 100 / 100.0f * 2 * PI;
					desc.EndRadian = desc.StartRadian + 2 * PI;
					desc.Radius = rand() % 100 / 100.0f * 6 + 3;
					desc.Duration = rand() % 100 / 100.0f * 360 + 720;
					desc.LoopCount = INT_MAX;
					new Anime_Position(animator, desc, computeFunc);
				}
			}
		}

		//few
		{

		}
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
}

void TitleScene::MainUnload()
{
	for (auto as : actSelect)
	{
		as = nullptr;
	}
}

void TitleScene::MainUpdate()
{
	//test
	{

	}

	//depth view draw
	{
		//shadow depth draw
		Renderer::BeginDepth();

		//ライトカメラの行列をセット
		LightManager::SelectMainLightView();
		//CameraManager::SelectMainCameraView();

		//影を落としたいオブジェクトを描画(一応地面も)
		DrawManager::DrawShadowDepth();
	}
}
