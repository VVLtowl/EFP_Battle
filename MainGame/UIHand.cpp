#include "main.h"
#include "LookAtCamera.h"
#include "UIHand.h"

#include "WorldToScreen.h"

const float X_NUM = 5.0f;
const float Y_NUM = 1.0f;
const float TEXCOORD_W = 1.0f / X_NUM;
const float TEXCOORD_H = 1.0f / Y_NUM;

UIHand::UIHand()
{
	//create ui follow worldObject
	{
		FollowWorldObject = new UIFollowWorldObject(this);
	}

	//create polygon ui
	{
		PolygonUIDescription desc;
		desc.DrawObjDesc.Shader = SHADER_UNLIT;
		desc.DrawObjDesc.DepthEnable = false;
		desc.DrawObjDesc.Layer = LAYER_SCREEN_UI;
		desc.DrawObjDesc.Texcoord = { 0,0,TEXCOORD_W ,TEXCOORD_H };
		desc.Size = UI_HAND_SIZE;
		desc.TextureID = TEXID_UI_RPS;
		m_HandImage = new PolygonUI(this, desc);
	}
}

void UIHand::UpdateGameObject()
{
}

void UIHand::SetHandType(HandType type)
{
	Type = type;

	float x = (int)type % (int)(X_NUM)*TEXCOORD_W;
	float y = (int)type / (int)(Y_NUM)*TEXCOORD_H;
	m_HandImage->Texcoord = { x,y,TEXCOORD_W,TEXCOORD_H };
}
