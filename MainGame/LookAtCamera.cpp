#include "main.h"
#include "LookAtCamera.h"

LookAtCamera::LookAtCamera(GameObject* owner, Camera* cmr, int order):
	Component(owner,order),
	TargetCamera(cmr)
{
}

LookAtCamera::~LookAtCamera()
{
}

void LookAtCamera::Update()
{
	m_Owner->GetTransform()->DisableUpdateThisFrame();
	D3DXMATRIX world;

	//if(TargetCamera->IsOrtho==false)
	if(0)
	{
		D3DXMATRIX rot;
		D3DXMatrixIdentity(&rot);
		D3DXVECTOR4 pos4 = TargetCamera->CameraData.Position;
		D3DXVECTOR4 dir4 = TargetCamera->CameraData.Direction;
		D3DXVECTOR3 pos3 = { pos4.x,pos4.y,pos4.z };
		D3DXVECTOR3 dir = { dir4.x,dir4.y,dir4.z };
		D3DXVECTOR3 eye = pos3;//pos;
		D3DXVECTOR3 lookAt = m_Owner->GetTransform()->GetWorldPosition() ;//eye + dir * TargetCamera->ZNear;
		D3DXMatrixLookAtLH(&rot, &eye, &lookAt, &V3_AXIS_Y);

		D3DXMatrixInverse(&rot, nullptr, &rot);
		rot._41 = 0;
		rot._42 = 0;
		rot._43 = 0;

		D3DXMATRIX translate, scale;
		D3DXVECTOR3 pos = m_Owner->GetTransform()->GetWorldPosition();
		D3DXVECTOR3 scl = m_Owner->GetTransform()->GetWorldScale();
		D3DXMatrixTranslation(&translate, pos.x, pos.y, pos.z);
		D3DXMatrixScaling(&scale, scl.x, scl.y, scl.z);
		world = scale * rot * translate;
	}
	else
	{
		D3DXVECTOR3 pos = m_Owner->GetTransform()->GetWorldPosition();
		D3DXVECTOR3 scl = m_Owner->GetTransform()->GetWorldScale();

		LookAtCameraMatrix lookAtCmrMtx;
		world = lookAtCmrMtx(pos, scl, TargetCamera);
	}

	m_Owner->GetTransform()->SetWorldMatrix(world);
}

D3DXMATRIX LookAtCameraMatrix::operator()(
	const D3DXVECTOR3& pos, 
	const D3DXVECTOR3& scl, 
	Camera* cmr)const
{
	D3DXMATRIX rot;
	D3DXMatrixIdentity(&rot);
	D3DXVECTOR4 pos4 = cmr->CameraData.Position;
	D3DXVECTOR4 dir4 = cmr->CameraData.Direction;
	D3DXVECTOR3 eye = { pos4.x,pos4.y,pos4.z };
	D3DXVECTOR3 dir = { dir4.x,dir4.y,dir4.z };
	D3DXVECTOR3 lookAt = eye + dir * cmr->ZNear;
	D3DXMatrixLookAtLH(&rot, &eye, &lookAt, &V3_AXIS_Y);

	D3DXMatrixInverse(&rot, nullptr, &rot);
	rot._41 = 0;
	rot._42 = 0;
	rot._43 = 0;

	D3DXMATRIX translate, scale;
	D3DXMatrixTranslation(&translate, pos.x, pos.y, pos.z);
	D3DXMatrixScaling(&scale, scl.x, scl.y, scl.z);

	return  scale * rot * translate;
}
