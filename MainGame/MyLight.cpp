#include "main.h"
#include "renderer.h"
#include "GameObject.h"
#include "Transform3D.h"
#include "MyLight.h"

#include "LookAt.h"
 
MyLight::MyLight(GameObject* owner, LightDescription litDesc, int order) :
	Component(owner,order)
{
	IsOrtho = litDesc.IsOrtho;
	LightData = litDesc.LightData;	
	Fovy = litDesc.Fovy;
	Width = litDesc.Width;
	Height = litDesc.Height;
	ZNear = litDesc.ZNear;
	ZFar = litDesc.ZFar;
	Angle = litDesc.Angle;

	//add lookAt to owner
	{
		LightLookAt = new LookAt(m_Owner);
	}
}

void MyLight::Update()
{
	//ライトカメラのビュー行列を作成
	Transform3D* trans = m_Owner->GetTransform();
	D3DXVECTOR3 pos = trans->GetPosition();
	D3DXVECTOR3 forwardPos = pos + trans->GetForward();
	D3DXVECTOR3 lookAtPos =
		(LightLookAt->m_TargetTransform == nullptr ? forwardPos : LightLookAt->m_TargetTransform->GetPosition());
	D3DXVECTOR3 up = V3_AXIS_Y;
	D3DXMatrixLookAtLH(
		&LightData.ViewMatrix, 
		&pos, 
		&lookAtPos, 
		&up
	);

	//視錐計算と設定

	if (IsOrtho)
	{
		//ライトカメラのプロジェクション行列を作成
		D3DXMatrixOrthoLH(
			&LightData.ProjectionMatrix,
			Width,
			Height,
			ZNear,
			ZFar
		);
	}
	else
	{
		float aspect = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		D3DXMatrixPerspectiveFovLH(
			&LightData.ProjectionMatrix,
			Fovy,
			aspect,
			ZNear,
			ZFar
		);
	}

	//compute direction
	D3DXVECTOR3 dir = lookAtPos - pos;
	D3DXVec3Normalize(&dir, &dir);
	LightData.Direction = D3DXVECTOR4(dir, 0);

	//set position
	LightData.Position = D3DXVECTOR4(pos, 1);

	//set angle
	//float radian = Angle * PI / 180.0f;
	float angleX = Angle * PI / 180.0f;// fabsf(cosf(radian));
	LightData.Angle = D3DXVECTOR4(angleX, 1, 1, 1);
}

void MyLight::Draw()
{
	////shadow depth draw
	//Renderer::BeginDepth();

	////ライトカメラの行列をセット
	//LightManager::SelectMainLightView();
	////CameraManager::SelectMainCameraView();

	////影を落としたいオブジェクトを描画(一応地面も)
	//DrawManager::DrawShadowDepth();

	//Renderer::Begin(this);
	//CAMERA camera = CameraData;
	//Renderer::SetViewMatrix(&camera.ViewMatrix);
	//Renderer::SetProjectionMatrix(&camera.ProjectionMatrix);

	//DrawManager::DrawWorldObject();
}
