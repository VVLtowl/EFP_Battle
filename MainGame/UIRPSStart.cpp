#include "main.h"
#include "UIRPSStart.h"

UIRPSStart::UIRPSStart()
{
	//create polygonUI
	{
		PolygonUIDescription desc;
		desc.DrawObjDesc.Layer = LAYER_SCREEN_START_UI;
		desc.Size = UI_RPS_START_SIZE;
		desc.TextureID = TEXID_UI_RPS_START;
		m_PolygonUI = new PolygonUI(this, desc);
	}
}

void UIRPSStart::UpdateGameObject()
{
}
