#include "main.h"
#include "LookAt.h"
#include "TransformAnime.h"
#include "FreeCamera.h"

FreeCamera::FreeCamera()
{
	//set name
	//SetName("FreeCamera");

	//create camera component
	{
		CameraDescription desc;
		desc.IsOrtho = true;
		desc.ZNear = 1;
		desc.ZFar = 100;
		desc.Width = 9;
		desc.Height = 9;
		m_Camera = new Camera(this, desc);

		//managed by CameraManager
		CameraManager::Register(m_Camera);
	}

	//set main camera
	CameraManager::Register(m_Camera);

	//create LookAt
	//m_LookAt = new LookAt(this);

	//create camera polygon
#if 1
	//Polygon3DDescription polyDesc;
	//polyDesc.DrawObjDesc.Shader = SHADER_UNLIT;
	//polyDesc.DrawObjDesc.DepthEnable = false;
	//polyDesc.ModelID = MDLID_CUBE;
	//m_Polygon3D = new Polygon3D(this, polyDesc);
	m_HintObject = GameObjectManager::Create<Cube>();
	m_HintObject->GetTransform()->SetParent(m_Transform3D);
	m_HintObject->GetTransform()->SetScale(V3_ONE * 0.05f);
	//m_HintObject->SetName("FreeCamera HintObject");
#endif

	//init anime
#if 0
	{
		TransformAnimeDescripition desc;
		desc.LoopCount = INT_MAX;
		desc.Duration = 240.0f;
		desc.StartPosition = m_Transform3D->GetPosition();
		desc.EndPosition = m_Transform3D->GetPosition();
		SineMoveAnime computeFunc(
			D3DXVECTOR3(+5, 14, -10),
			D3DXVECTOR3(-5, 14, -10),
			-PI / 2,
			PI * 3 / 2);
		desc.ComputeAniDataFunc = &computeFunc;
		m_TransformAnime = new TransformAnime(this, desc);
	}
#endif
}

FreeCamera::~FreeCamera()
{
	CameraManager::Quit(m_Camera);
}

void FreeCamera::UpdateGameObject()
{	
	//update render main camera data
	//Renderer::SetCamera(m_Camera->CameraData);
}

//void FreeCamera::SetLookAt(Transform3D* target)
//{
//	m_LookAt->TargetTransform = target;
//	m_Camera->LookAt = target;
//}
