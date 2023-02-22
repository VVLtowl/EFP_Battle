#include "main.h"
#include "UIActStart.h"

const D3DXVECTOR2 UI_GAME_ACT_SIZE = { 1280.0f / 3.0f, 1280.0f / 3.0f };

UIActStart::UIActStart()
{	
	//create polygonUI
	{
		PolygonUIDescription desc;
		desc.DrawObjDesc.Layer = LAYER_SCREEN_START_UI;
		desc.Size = UI_GAME_ACT_SIZE;
		desc.TextureID = TEXID_UI_ACT_START;//default
		m_PolygonUI = new PolygonUI(this, desc);
	}
}

void UIActStart::UpdateGameObject()
{
}
