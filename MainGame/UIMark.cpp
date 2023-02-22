#include "main.h"
#include "UIMark.h"

#include "TransformAnime.h"

const float X_NUM = 5.0f;
const float Y_NUM = 1.0f;
const float TEXCOORD_W = 1.0f / X_NUM;
const float TEXCOORD_H = 1.0f / Y_NUM;


UIMark::UIMark()
{
	//create polygonUI
	{
		PolygonUIDescription desc;
		desc.DrawObjDesc.Shader = SHADER_UNLIT;
		desc.DrawObjDesc.DepthEnable = false;
		desc.DrawObjDesc.Layer = LAYER_SCREEN_UI;
		desc.DrawObjDesc.Texcoord = { 0,0,TEXCOORD_W ,TEXCOORD_H };
		desc.Size = UI_MARK_SIZE;
		desc.TextureID = TEXID_UI_MARK;
		m_MarkUI = new PolygonUI(this, desc);
	}

	//set up anime
	{
		//create animator comp
		m_Animator = new Animator(this);

		//create anime
		AniDesc_Texture aniDesc;
		aniDesc.LoopCount = INT_MAX;
		aniDesc.Duration = 60;
		aniDesc.StartTexcoord = {
			TEXCOORD_W * 2,
			0,
			TEXCOORD_W / 3.0f,
			TEXCOORD_H / 3.0f };
		aniDesc.ColumnNum = 3;
		aniDesc.RowNum = 3;
		aniDesc.ClipNum = 8;
		new Anime_Texture(m_Animator, aniDesc, m_MarkUI);

		m_Animator->SetState(Component::PAUSED);
	}
}

void UIMark::UpdateGameObject()
{
}

void UIMark::SetMark(Type type)
{
	m_Type = type;
	float x = (int)type % (int)(X_NUM)*TEXCOORD_W;
	float y = (int)type / (int)(Y_NUM)*TEXCOORD_H;
	m_MarkUI->Texcoord = { x,y,TEXCOORD_W,TEXCOORD_H };


	//special process
	if (type==Type::THINK)
	{
		m_Animator->SetState(Component::ACTIVE);
	}
	else
	{
		m_Animator->SetState(Component::PAUSED);
	}
}

