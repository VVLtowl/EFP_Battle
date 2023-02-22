#include "main.h"
#include "UISelect.h"

#include "TransformAnime.h"

const D3DXVECTOR2 UI_SELECT_SIZE = V2_ONE * 0.9f;

UISelect::UISelect()
{
	//create polygonUI
	{
		Polygon2DDescription desc;
		desc.DrawObjDesc.DepthEnable = false;
		desc.DrawObjDesc.Layer = LAYER_WORLD_PLAYER_ACTMARK;
		desc.DrawObjDesc.Shader = SHADER_UNLIT;
		desc.TextureID = TEXID_HINT2;
		desc.Size = UI_SELECT_SIZE;
		m_Polygon2D = new Polygon2D(this, desc);
	}
}

UISelect::~UISelect()
{
}

void UISelect::UpdateGameObject()
{
}

void UISelect::SetHighlightAnime(bool show)
{
	if (show)
	{
		if (m_Animator)
		{
			m_Animator->SetState(Component::DEAD);
			m_Animator = nullptr;
		}

		m_Animator = new Animator(this);
		AniDesc_Vec3Hermite sclDesc;
		{
			sclDesc.LoopCount = INT_MAX;
			sclDesc.Duration = 80;
			sclDesc.StartVec3 = V3_ONE;
			sclDesc.EndVec3 = V3_ONE;
			sclDesc.StartTangent = -V3_ONE;
			sclDesc.EndTangent = -V3_ONE;
		}
		ComputeHermiteVec3 computeFunc;
		new Anime_Scale(m_Animator, sclDesc, computeFunc);
	}
	else
	{
		if (m_Animator)
		{
			m_Animator->SetState(Component::DEAD);
			m_Animator = nullptr;
		}
	}
}
