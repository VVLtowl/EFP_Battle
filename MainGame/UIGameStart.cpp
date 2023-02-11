#include "main.h"
#include "UIGameStart.h"

UIGameStart::UIGameStart()
{
	//create polygonUI
	{
		PolygonUIDescription desc;
		desc.DrawObjDesc.Layer = LAYER_SCREEN_START_UI;
		desc.Size = UI_GAME_START_SIZE;
		desc.TextureID = TEXID_UI_GAME_START;
		m_PolygonUI = new PolygonUI(this, desc);
	}
}

void UIGameStart::UpdateGameObject()
{

}
