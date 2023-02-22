#include "main.h"
#include "WorldToScreen.h"

UIFollowWorldObject::UIFollowWorldObject(
	GameObject* owner,
	Transform3D* followTarget,
	int order):
	Component(owner,order),
	TargetTransform(followTarget)
{

}

UIFollowWorldObject::~UIFollowWorldObject()
{
	TargetTransform = nullptr;
}

void UIFollowWorldObject::Update()
{
	if (TargetTransform != nullptr)
	{
		D3DXVECTOR3 worldPos = TargetTransform->GetWorldPosition();
		WorldPositionToScreenPosition computeFunc;
		D3DXVECTOR2 screenPos = computeFunc(worldPos);

		m_Owner->GetTransform()->SetPosition(screenPos.x, screenPos.y, 0);
	}
}

D3DXVECTOR2& WorldPositionToScreenPosition::operator()(const D3DXVECTOR3& worldPos) const
{
	Camera* cmr = CameraManager::GetMainCamera();
	D3DXVECTOR2 screenPos = { SCREEN_WIDTH_HALF,SCREEN_HEIGHT_HALF };

	if (cmr->m_IsOrtho == true)
	{
		D3DXMATRIX worldTranslate,translate,rotate,scale;
		D3DXMATRIX viewTranslate;
		D3DXMatrixTranslation(&translate, worldPos.x, worldPos.y, worldPos.z);
		//D3DXMatrixRotationYawPitchRoll(&rotate, 0, 0, 0);
		//D3DXMatrixScaling(&scale, 1, 1, 1);
		//worldTranslate = scale * rotate * translate;
		worldTranslate = translate;
		D3DXMATRIX cmrWorldInv;
		D3DXMATRIX cmrWorld= cmr->GetGameObject()->GetTransform()->GetWorldMatrix();
		//D3DXMatrixInverse(&viewInv, NULL, &cmr->CameraData.ViewMatrix);
		D3DXMatrixInverse(&cmrWorldInv, NULL, &cmrWorld);
		viewTranslate =  worldTranslate * cmrWorldInv;
		D3DXVECTOR2 viewPos = { viewTranslate._41 ,viewTranslate._42 };
		float tx = viewPos.x / cmr->m_Width + 0.5f;
		float ty = -viewPos.y / cmr->m_Height  + 0.5f;
		screenPos = { SCREEN_WIDTH * tx,SCREEN_HEIGHT * ty };
	}
	else
	{
		D3DXMATRIX worldTranslate, translate, rotate, scale;
		D3DXMATRIX cmrWorldTranslate;
		D3DXMatrixTranslation(&translate, worldPos.x, worldPos.y, worldPos.z);
		worldTranslate = translate;
		D3DXMATRIX cmrWorldInv;
		D3DXMATRIX cmrWorld = cmr->GetGameObject()->GetTransform()->GetWorldMatrix();
		D3DXMatrixInverse(&cmrWorldInv, NULL, &cmrWorld);
		cmrWorldTranslate = worldTranslate * cmrWorldInv;

		float zFar = cmrWorldTranslate._43;
		float zHeight = tanf(cmr->m_Fovy / 2 * PI / 180) * zFar * 2;
		float zWidth = zHeight * cmr->m_Width / cmr->m_Height;
		D3DXVECTOR2 cmrWorldPos = { cmrWorldTranslate._41 ,cmrWorldTranslate._42 };
		float tx = cmrWorldPos.x / zWidth + 0.5f;
		float ty = -cmrWorldPos.y / zHeight + 0.5f;
		screenPos = { SCREEN_WIDTH * tx,SCREEN_HEIGHT * ty };
	}

	return screenPos;
}
