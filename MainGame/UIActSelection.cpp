#include "main.h"
#include "UIActSelection.h"

UIActSelection::UIActSelection()
{
	//create polygon2d
	{
		Polygon2DDescription desc;
		desc.DrawObjDesc.Shader = SHADER_UNLIT;
		desc.DrawObjDesc.DepthEnable = false;
		desc.DrawObjDesc.Layer = LAYER_WORLD_BUTTON;
		desc.DrawObjDesc.Texcoord = { 0,0,1,1 };
		desc.Size = UI_ACTSELECTION_SIZE;
		desc.TextureID = TEXID_UI_MARK;
		m_ActSelectionUI = new Polygon2D(this, desc);
	}
}

void UIActSelection::UpdateGameObject()
{
}

void UIActSelection::SetActTex(int texid)
{
	m_ActSelectionUI->Texture = DrawManager::Textures[texid];
}
