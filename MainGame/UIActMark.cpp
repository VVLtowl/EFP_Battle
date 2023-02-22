#include "main.h"
#include "UIActMark.h"

#include "TransformAnime.h"
#include "WorldToScreen.h"

UIActMark::UIActMark(Transform3D* targetTrans)
{
	//create UI
	{
		m_UIMark = new UIMark();
		m_UIMark->SetMark(UIMark::Type::ACT);
		m_UIMark->SetParent(this);
	}

	//create ui follow obj
	{
		m_FollowWorldObject = new UIFollowWorldObject(this);
		m_FollowWorldObject->SetTargetTransform(targetTrans);
	}

	//make anime
	{
		Animator* animator = new Animator(m_UIMark);

		//scaleAnime
		AniDesc_Vec3Hermite sclDesc;
		sclDesc.Duration = 30;
		sclDesc.LoopCount = INT_MAX;
		sclDesc.StartVec3 = V3_ONE * 0.5f;
		sclDesc.EndVec3 = V3_ONE * 0.5f;
		sclDesc.StartTangent = -V3_ONE * 0.9f;
		sclDesc.EndTangent = V3_ONE * 0.9f;
		ComputeHermiteVec3 sclComputeFunc;
		new Anime_Scale(animator, sclDesc, sclComputeFunc);
	}
}

UIActMark::~UIActMark()
{
}

void UIActMark::UpdateGameObject()
{
}
