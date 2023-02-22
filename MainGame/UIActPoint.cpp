#include "main.h"
#include "UIActPoint.h"

#include "WorldToScreen.h"
#include "TransformAnime.h"

const float UI_ACTPOINT_SIZE_RATIO_ORIGINAL = 0.4f;
const float UI_ACTPOINT_SIZE_RATIO_HIGHLIGHT = 0.6f;

UIActPoint::UIActPoint()
{
	//create follow world object
	{
		m_FollowWorldObject = new UIFollowWorldObject(this);
	}

	//create ui numbers
	{
		m_OwnUINumbers = GameObjectManager::Create<UINumbers>();
		m_OwnUINumbers->SetParent(this);
		m_OwnUINumbers->GetTransform()->SetScale(V3_ONE * UI_ACTPOINT_SIZE_RATIO_ORIGINAL);
		m_OwnUINumbers->SetNumber(0);
		//StartNumberChangeAnime(0);
	}
}

UIActPoint::~UIActPoint()
{
	//OwnUINumbers->SetState(GameObject::DEAD);
}

void UIActPoint::UpdateGameObject()
{
}

void UIActPoint::StartNumberChangeAnime(int num)
{
	//first turn bigger
	if(m_OwnUINumbers)
	{
		Animator* animator1 = new Animator(m_OwnUINumbers);
		animator1->AddEndEvent([num,this]()
			{
				m_OwnUINumbers->SetNumber(num);

				//second turn back original 
				{
					Animator* animator2 = new Animator(m_OwnUINumbers);

					//posAnime
					AniDesc_Vec3StartEnd posDesc;
					posDesc.Duration = 10;
					posDesc.Start = V3_ZERO;// { 0, -UI_NUMBER_SIZE.y * UI_ACTPOINT_SIZE_RATIO_ORIGINAL / 2, 0 };
					posDesc.End = V3_ZERO;// { 0, -UI_NUMBER_SIZE.y * UI_ACTPOINT_SIZE_RATIO_ORIGINAL / 2, 0 };
					ComputeSlowStartSlowEndVec3 posComputeFunc;
					new Anime_Position(animator2, posDesc, posComputeFunc);

					//scaleAnime
					AniDesc_Vec3StartEnd sclDesc;
					sclDesc.Duration = 10;
					sclDesc.Start = V3_ONE * UI_ACTPOINT_SIZE_RATIO_HIGHLIGHT;
					sclDesc.End = V3_ONE * UI_ACTPOINT_SIZE_RATIO_ORIGINAL;
					ComputeSlowStartSlowEndVec3 sclComputeFunc;
					new Anime_Scale(animator2, sclDesc, sclComputeFunc);
				}
			});

		//posAnime
		AniDesc_Vec3StartEnd posDesc;
		posDesc.Duration = 10;
		posDesc.Start = V3_ZERO;// { 0, -UI_NUMBER_SIZE.y * UI_ACTPOINT_SIZE_RATIO_ORIGINAL / 2, 0 };
		posDesc.End = V3_ZERO;// { 0, -UI_NUMBER_SIZE.y * UI_ACTPOINT_SIZE_RATIO_ORIGINAL / 2, 0 };
		ComputeSlowStartSlowEndVec3 posComputeFunc;
		new Anime_Position(animator1, posDesc, posComputeFunc);

		//scaleAnime
		AniDesc_Vec3StartEnd sclDesc;
		sclDesc.Duration = 10;
		sclDesc.Start = V3_ONE * UI_ACTPOINT_SIZE_RATIO_ORIGINAL;
		sclDesc.End = V3_ONE * UI_ACTPOINT_SIZE_RATIO_HIGHLIGHT;
		ComputeSlowStartSlowEndVec3 sclComputeFunc;
		new Anime_Scale(animator1, sclDesc, sclComputeFunc);
	}
}
