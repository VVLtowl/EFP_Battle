#include "main.h"
#include "UINumber.h"

const float X_NUM = 5.0f;
const float Y_NUM = 5.0f;
const float TEXCOORD_W = 1.0f / X_NUM;
const float TEXCOORD_H = 1.0f / Y_NUM;

UINumber::UINumber(LayerType layer)
{
	//create polygonUI
	{
		PolygonUIDescription desc;
		desc.DrawObjDesc.Layer = layer;
		desc.Size = UI_NUMBER_SIZE;
		desc.TextureID = TEXID_NUMBER_0;
		m_NumberUI = new PolygonUI(this, desc);

		SetNumber(0);
	}
}

void UINumber::UpdateGameObject()
{
}

void UINumber::SetNumber(int num)
{
	int x = num % (int)(X_NUM);
	int y = num / (int)(X_NUM);
	m_NumberUI->Texcoord = 
	{ x * TEXCOORD_W,y * TEXCOORD_H,
		TEXCOORD_W,TEXCOORD_H };
}
