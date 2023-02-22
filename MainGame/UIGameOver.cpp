#include "main.h"
#include "UIGameOver.h"

UIGameOver::UIGameOver(ResultType result)
{
	//create polygonUI
	{
		PolygonUIDescription desc;
		desc.DrawObjDesc.Layer = LAYER_SCREEN_START_UI;
		desc.Size = UI_GAME_OVER_SIZE;
		if (ResultType::BAD_WIN == result)
		{
			desc.TextureID = TEXID_UI_GAMEOVER_BADWIN;
		}
		else if (ResultType::GOOD_WIN == result)
		{
			desc.TextureID = TEXID_UI_GAMEOVER_GOODWIN;
		}
		m_PolygonUI = new PolygonUI(this, desc);
	}
}

void UIGameOver::UpdateGameObject()
{

}