#include "main.h"
#include "LookAtCamera.h"
#include "UIRPS.h"

const float X_NUM = 5.0f;
const float Y_NUM = 1.0f;
const float TEXCOORD_W = 1.0f / X_NUM;
const float TEXCOORD_H = 1.0f / Y_NUM;

UIRPS::UIRPS()
{
	//create polygon2d
	{
		Polygon2DDescription desc;
		desc.DrawObjDesc.Shader = SHADER_UNLIT;
		desc.DrawObjDesc.DepthEnable = false;
		desc.DrawObjDesc.Layer = LAYER_WORLD_BUTTON;
		desc.DrawObjDesc.Texcoord = { 0,0,TEXCOORD_W ,TEXCOORD_H };
		desc.Size = UI_RPS_SIZE;
		desc.TextureID = TEXID_UI_RPS;
		m_RPSUI = new Polygon2D(this, desc);
	}
}

void UIRPS::UpdateGameObject()
{
}

void UIRPS::SetRPS(int type)
{
	float x = type % (int)(X_NUM)*TEXCOORD_W;
	float y = type / (int)(Y_NUM)*TEXCOORD_H;
	m_RPSUI->Texcoord = { x,y,TEXCOORD_W,TEXCOORD_H };
}
