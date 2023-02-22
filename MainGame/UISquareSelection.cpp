#include "main.h"
#include "UISquareSelection.h"

UISquareSelection::UISquareSelection()
{
	//create polygon2d
	{
		Polygon2DDescription desc;
		desc.DrawObjDesc.Shader = SHADER_UNLIT;
		desc.DrawObjDesc.DepthEnable = false;
		desc.DrawObjDesc.Layer = LAYER_WORLD_BUTTON;
		desc.DrawObjDesc.Texcoord = { 0,0,1,1 };
		desc.Size = UI_SQUARESELECTION_SIZE;
		desc.TextureID = TEXID_UI_SQUARE;
		m_Polygon2D = new Polygon2D(this, desc);
	}
}

void UISquareSelection::UpdateGameObject()
{
}
