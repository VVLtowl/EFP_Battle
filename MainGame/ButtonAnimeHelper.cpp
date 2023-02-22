#include "main.h"
#include "Button.h"
#include "TransformAnime.h"
#include "ButtonAnimeHelper.h"

//ButtonAnimeDescripition ButtonAnimeDesc;

ButtonHelper::ButtonHelper(
	GameObject* uiObject,
	Button* btn, 
	Polygon2D* uiPolygon):
	UIObject(uiObject),
	UIButton(btn),
	UIPolygon2D(uiPolygon),
	TargetAnimator(nullptr)
{
}

ButtonHelper::~ButtonHelper()
{
	UIObject = nullptr;
	UIButton = nullptr;
	UIPolygon2D = nullptr;
}

void ButtonHelper::InitButtonAnime()
{
	UIButton->AddDisableEvent([this]() 
		{
			UIPolygon2D->Color = m_ButtonAniDesc.DisableUIColor;
		});
	UIButton->AddEnableEvent([this]() 
		{
			UIPolygon2D->Color = m_ButtonAniDesc.OriginalUIColor;
		});

	UIButton->AddEnterEvent([this]()
		{
			UIPolygon2D->Color = m_ButtonAniDesc.HighlightUIColor;

			if (TargetAnimator != nullptr)
			{
				TargetAnimator->SetState(Component::DEAD);
				//DebugInfo::Print(UIObject->GetName() + " clear DEAD:");
			}
			//DebugInfo::Print(UIObject->GetName() + " show start");
			TargetAnimator = new Animator(UIObject);
			TargetAnimator->AddEndEvent([this]() {
				TargetAnimator = nullptr;
				//DebugInfo::Print(UIObject->GetName()+" show finish");
				});

			AniDesc_Vec3StartEnd desc;
			ComputeSlowLerpVec3 computeFunc;
			desc.Duration = 15;

			desc.Start = UIObject->GetTransform()->GetScale();
			desc.End = m_ButtonAniDesc.HighlightUIScale;
			Anime_Scale* sclAnime = new Anime_Scale(TargetAnimator, desc, computeFunc);

			desc.Start = UIObject->GetTransform()->GetPosition();
			desc.End = m_ButtonAniDesc.HighlightUIPosition;
			Anime_Position* posAnime = new Anime_Position(TargetAnimator, desc, computeFunc);
		});
	UIButton->AddExitEvent([this]()
		{
			UIPolygon2D->Color = m_ButtonAniDesc.OriginalUIColor;
			
			if (TargetAnimator != nullptr)
			{
				//if (TargetAnimator->GetState() == Component::DEAD)
				//{
				//	DebugInfo::Print(UIObject->GetName() + " target animetor error");
				//}
				TargetAnimator->SetState(Component::DEAD);
				//DebugInfo::Print(UIObject->GetName() + " show DEAD:");
			}
			//DebugInfo::Print(UIObject->GetName() + " clear start");
			TargetAnimator = new Animator(UIObject);
			TargetAnimator->AddEndEvent([this]() {
				TargetAnimator = nullptr;
				//DebugInfo::Print(UIObject->GetName() + " clear finish");
				});

			AniDesc_Vec3StartEnd desc;
			ComputeSlowLerpVec3 computeFunc;
			desc.Duration = 15;

			desc.Start = UIObject->GetTransform()->GetScale();
			desc.End = m_ButtonAniDesc.OriginalUIScale;
			Anime_Scale* sclAnime = new Anime_Scale(TargetAnimator, desc, computeFunc);

			desc.Start = UIObject->GetTransform()->GetPosition();
			desc.End = m_ButtonAniDesc.OriginalUIPosition;
			Anime_Position* posAnime = new Anime_Position(TargetAnimator, desc, computeFunc);
		});
	UIButton->AddClickEvent([&]()
		{
			UIPolygon2D->Color = m_ButtonAniDesc.ClickUIColor;
			
			if (TargetAnimator != nullptr)
			{
				TargetAnimator->SetState(Component::DEAD);
			}
			TargetAnimator = new Animator(UIObject);

			AniDesc_Vec3StartEnd desc;
			ComputeSlowLerpVec3 computeFunc;
			desc.Duration = 10;

			desc.Start = UIObject->GetTransform()->GetScale();
			desc.End = m_ButtonAniDesc.ClickUIScale;
			Anime_Scale* sclAnime = new Anime_Scale(TargetAnimator, desc, computeFunc);

			desc.Start = UIObject->GetTransform()->GetPosition();
			desc.End = m_ButtonAniDesc.ClickUIPosition;
			Anime_Position* posAnime = new Anime_Position(TargetAnimator, desc, computeFunc);
		});
}

void ButtonHelper::StartUIAppear()
{
}

void ButtonHelper::StartUIDisappear()
{
}
