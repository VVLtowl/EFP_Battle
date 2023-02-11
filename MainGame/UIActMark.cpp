#include "main.h"
#include "UIActMark.h"

#include "TransformAnime.h"

UIActMark::UIActMark()
{
	//create UI
	{
		OwnUISelect = GameObjectManager::Create<UISelect>();
		OwnUISelect->GetTransform()->SetParent(m_Transform3D);
	}

	//make anime
	{
		Animator* animator = new Animator(OwnUISelect);

		//scaleAnime
		AniDesc_Vec3Hermite sclDesc;
		sclDesc.Duration = 30;
		sclDesc.LoopCount = INT_MAX;
		sclDesc.StartVec3 = V3_ONE * 1.8f;
		sclDesc.EndVec3 = V3_ONE * 1.8f;
		sclDesc.StartTangent = -V3_ONE * 0.9f;
		sclDesc.EndTangent = V3_ONE * 0.9f;
		ComputeHermiteVec3 sclComputeFunc;
		new Anime_Scale(animator, sclDesc, sclComputeFunc);
	}
}

UIActMark::~UIActMark()
{
	OwnUISelect->SetState(GameObject::DEAD);
}

void UIActMark::UpdateGameObject()
{
}
