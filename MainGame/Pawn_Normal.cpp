#include "main.h"
#include "Pawn_Normal.h"
#include "LookAtCamera.h"

const float X_NUM = 10.0f;
const float Y_NUM = 1.0f;
const float TEXCOORD_W = 1.0f / X_NUM;
const float TEXCOORD_H = 1.0f / Y_NUM;

Pawn_Normal::Pawn_Normal()
{
	//create polygon2d
	{
		Polygon2DDescription desc;
		desc.DrawObjDesc.Layer = LAYER_WORLD_SHADOW_PLAYER;
		desc.DrawObjDesc.Shader = SHADER_SHADOW;
		desc.DrawObjDesc.DepthEnable = true;
		desc.DrawObjDesc.Texcoord = { 0,0,TEXCOORD_W ,TEXCOORD_H };
		desc.Size = PIECE_IMAGE_SIZE;
		desc.TextureID = TEXID_FIELD004;
		PieceImage = new Polygon2D(this, desc);
	}

	//create look at camera
	{
		LookAtCmr =
			new LookAtCamera(this, CameraManager::GetMainCamera());
	}
}

void Pawn_Normal::UpdateGameObject()
{
	//look at camera
#if 0
	//test
	D3DXMATRIX view =
		CameraManager::GetMainCamera()->CameraData.ViewMatrix;
	m_Transform3D->LookAtView(view);
	//m_Transform3D->LookAt(CameraManager::GetMainCamera()->GetGameObject()->GetTransform()->GetPosition());
#endif

	//update tex anime
#if 1
#else
#endif
}