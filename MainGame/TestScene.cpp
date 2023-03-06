#include "main.h"
#include "TestScene.h"
#include "LookAt.h"
#include "LookAtCamera.h"
#include "MoveControl.h"
#include "Button.h"

#include "Board.h"
#include "Judgement.h"
#include "Server.h"
#include "Client.h"

#include "ButtonObject.h"

#include "ServerBehaviour.h"
#include "ClientBehaviour.h"

#include "SceneManager.h"
#include "TransformAnime.h"

#include "imgui/imgui.h"


/*********************************************************
* @brief	for test
********************************************************/
#include "Piece.h"
#include "PieceObject.h"
#include "PieceBehaviour.h"

//#define TEST_BEHAVIOUR

void TestScene::MainLoad()
{
	//InitGameObjectManager();

	//test mark anime
	if (0)
	{
		UIMark* ui = new UIMark();
		ui->SetMark(UIMark::Type::THINK);
		ui->GetTransform()->SetPosition(SCREEN_WIDTH_HALF, SCREEN_HEIGHT_HALF, 0);


		PieceObject* pieceObj = new PieceObject();
		pieceObj->m_PawnImage->m_Polygon2D->Texture = DrawManager::Textures[TEXID_HINT];

		//new LookAtCamera(pieceObj, CameraManager::GetMainCamera());

		UISelect* selectMark = new UISelect();
		selectMark->GetTransform()->SetRotation(PI / 2, 0, 0);
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
		plane->GetTransform()->SetScale(2, 0, 1);

		//test set main camera look at plane pos
		CameraManager::GetMainCamera()->m_CmrLookAt->m_TargetTransform = plane->GetTransform();
		//test set main light look at plane pos
		LightManager::GetMainLight()->LightLookAt->m_TargetTransform = plane->GetTransform();

		TransformObject* lookAtTrs = GameObjectManager::Create<TransformObject>();
		lookAtTrs->GetTransform()->SetPosition(0, 0, -1);
		lookAtTrs->GetTransform()->SetParent(plane->GetTransform());

		SquareObject* sqrObj = new SquareObject();
		sqrObj->m_Polygon3D->Model = DrawManager::Models[MDLID_SQUARE_PRISON];
		sqrObj->m_Polygon3D->Model = DrawManager::Models[MDLID_SQUARE_GOAL];

		if(0)
		{
			FreeCamera* cmr = GameObjectManager::Create<FreeCamera>();
			cmr->GetTransform()->SetPosition(0, 0, -0.5f);
			cmr->GetTransform()->SetParent(plane->GetTransform());

			plane->Image->Texture = cmr->GetCamera()->m_MainSRV;
			plane->Image->Texcoord = { 1,0,-1,1 };
			cmr->GetCamera()->m_CmrLookAt->m_TargetTransform = lookAtTrs->GetTransform();
		}
	}

	//test sky
	if (1)
	{
		Sphere* sky = GameObjectManager::Create<Sphere>(); 
		//sky->SetName("Sky");
	}

	//test cube
	if (0)
	{
		Cube* cube = GameObjectManager::Create<Cube>();
		cube->GetTransform()->SetPosition(0, 2, 0);
		cube->SetName("Cube");
		MoveControl* move = new MoveControl(cube);
	}

	//test cube and collision
	if (0)
	{
		//many
		if (1)
		{
			for (int i = 0; i < 100; i++)
			{
				Cube* cb = GameObjectManager::Create<Cube>();
				//cb->GetTransform()->SetRotation(rand() % 100 / 100.0f * PI / 3, rand() % 100 / 100.0f * PI / 3, 0);
				cb->GetTransform()->SetScale(rand() % 100 / 100.0f * 4.0f, rand() % 100 / 100.0f*4.0f, rand() % 100 / 100.0f * 2.9f);

				new LookAtCamera(cb,CameraManager::GetMainCamera());

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
			}
		}

		//few
		{

		}
	}

	//test create obj
	if (0)
	{
		//test plane
		Plane* plane = GameObjectManager::Create<Plane>();
		plane->GetTransform()->SetRotation(0, 0, 0);
		plane->GetTransform()->SetPosition(0, 0, 0);
		plane->GetTransform()->SetScale(0, 0, 0);
		//new LookAtCamera(plane, CameraManager::GetMainCamera());
		FreeCamera* freeCmr = new FreeCamera();
		//freeCmr->SetParent(CameraManager::GetMainCamera()->GetGameObject());
		freeCmr->GetTransform()->SetPosition(10, 16, 10);
		freeCmr->GetCamera()->m_CmrLookAt->m_TargetTransform = plane->GetTransform();
		//plane->Image->Texture = freeCmr->GetCamera()->MainSRV;

		//test obj
		Torus* t = GameObjectManager::Create<Torus>();
		Torus* t2 = GameObjectManager::Create<Torus>();
		Torus* t3 = GameObjectManager::Create<Torus>();
		//new MoveControl(t3);
		//Emitter* emt = GameObjectManager::Create<Emitter>();

		t2->GetTransform()->SetParent(t->GetTransform());
		t2->GetTransform()->SetPosition(1, 1, 0);
		t2->GetTransform()->SetScale(0.5f, 0.5f, 0.5f);

		t3->GetTransform()->SetParent(t2->GetTransform());
		t3->GetTransform()->SetPosition(0, 1, 0);
		t3->GetTransform()->SetScale(0.5f, 0.5f, 0.5f);


		//test line painter
		{
			DrawLineDescription dldesc;
			dldesc.StartTransform = t2->GetTransform();
			dldesc.EndTransform = t->GetTransform();
			dldesc.Color = { 5,2,2,1 };
			dldesc.Size = 0.01f;
			LinePainter::Instance()->MakeLine(dldesc);
		}
		{
			DrawLineDescription dldesc;
			dldesc.StartTransform = t3->GetTransform();
			dldesc.EndTransform = t2->GetTransform();
			dldesc.Color = { 5,2,2,1 };
			dldesc.Size = 0.01f;
			LinePainter::Instance()->MakeLine(dldesc);
		}

		//test emitter and local forward
		Emitter* emt = GameObjectManager::Create<Emitter>();
		emt->GetTransform()->SetParent(t3->GetTransform());

		//test pawn normal
		Pawn_Normal* pawn = GameObjectManager::Create<Pawn_Normal>();
		pawn->GetTransform()->SetParent(t2->GetTransform());


		//test transform anime
		Animator* animator = new Animator(t);
		{
			AniDesc_PositionCircleRotate desc;
			desc.LoopCount = INT_MAX;
			desc.Duration = 80;
			desc.StartRadian = PI / 2;
			desc.EndRadian = PI * 2.5f;
			desc.Radius = 2;
			desc.Center = { 1,2,0 };
			ComputePositionCircleRotate computeFunc;
			//new Anime_Position(animator, desc, computeFunc);
		}
	}
	if (0)
	{
		//test line collision
		LineCollisionTester* lct = GameObjectManager::Create<LineCollisionTester>();

		//test cube and collision
		Cube* cb = GameObjectManager::Create<Cube>();
		cb->GetTransform()->SetPosition(0, 3, -1);
		cb->GetTransform()->SetRotation(PI / 3, PI / 3, 0);
		cb->GetTransform()->SetScale(2, 2, 1);
		//new LookAtCamera(cb, cmr->GetCamera());


		//test ActSelection button
		D3DXVECTOR3 actSelectPos = { 0,0,0.5f };
		D3DXVECTOR3 actSelectScl = V3_ONE;
		ActSelection* actSelect = new ActSelection(actSelectPos, actSelectScl);
	}

	//test particle system
	if (0)
	{
		ParticleSystem* ps = GameObjectManager::Create<ParticleSystem>();
		ps->GetTransform()->SetPosition(0, 0, 0);
		MoveControl* move = new MoveControl(ps);
		move->TargetTransform = ps->GetShooter();
	}



#ifdef  TEST_BEHAVIOUR
		//test piece object
	//tips: need GameManager::Clear in unload
	if (1)
	{
		Judgement::Instance()->Init();
		Judgement::Instance()->ShuffleAndSetPiecesToPlayers();
		Judgement::Instance()->PrepareBoardAndPieces();



		//test PieceShowCheckActpoint
		Judgement* jud = Judgement::Instance();

		Piece* piece = GameManager::m_Pieces[0];
		//piece->AddHand(HandType::SCISSORS);
		piece->AddHand(HandType::PAPER);
		piece->SetUIActpoint(true);


		//test set other piece hands
		{
			Piece* rival1 = GameManager::m_Pieces[5];
			rival1->AddHand(HandType::ROCK);
			GameManager::m_Goods.emplace_back(rival1);
		}


		//piece->SetUIHands(true);
		Camera* cmr = CameraManager::GetMainCamera();
		Board* board = Board::Instance();
		PieceShowCheckActpoint* showCheckActpoint = new PieceShowCheckActpoint(piece, 10);
		piece->StartBH(showCheckActpoint);
	}
#endif //  TEST_BEHAVIOUR

	//test button
	if(1)
	{
		ActSelection* buttonStartServer = new ActSelection();
		ActSelection* buttonStartClient = new ActSelection();

		buttonStartServer->GetTransform()->SetPosition(-2, 2, 0); 
		buttonStartServer->m_Polygon2D->Texture = DrawManager::Textures[TEXID_UI_SERVER];
		buttonStartServer->m_Polygon2D->Color =
			buttonStartServer->OriginalColor = { 1,0,0,1 };
		buttonStartServer->m_Button->AddClickEvent([]()
			{
				//judgement work in server side
				Judgement::Instance()->Init();

				//create server
				AppServer* server=AppServer::Instance();
				
				//logic entrance
				server->Init();

				//app server bh start in next scene
				SceneManager::ChangeScene<ServerSettingScene>();
			});

		buttonStartClient->GetTransform()->SetPosition(2, 2, 0); 
		buttonStartClient->m_Polygon2D->Texture = DrawManager::Textures[TEXID_UI_CLIENT];
		buttonStartClient->m_Polygon2D->Color =
			buttonStartClient->OriginalColor = { 0,0,1,1 };
		buttonStartClient->m_Button->AddClickEvent([]()
			{
				//create client
				AppClient* client = AppClient::Instance();

				//logic entrance
				client->Init();
				//client->StartBH(client->BH_Init);

				//next bh start in next scene
				SceneManager::ChangeScene<ClientWaitScene>();
			});
	}
}

void TestScene::MainUnload()
{
#ifdef  TEST_BEHAVIOUR
	//test
	//AppClient::Instance()->ClearDataInGameScene();
	Judgement::Instance()->GameEnd();
#endif //  TEST_BEHAVIOUR

}

void TestScene::MainUpdate()
{
}
