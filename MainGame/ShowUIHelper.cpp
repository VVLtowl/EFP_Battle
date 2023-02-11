#include "main.h"
#include "TransformAnime.h"
#include "ShowUIHelper.h"

#pragma region ========== step ui anime helper ==========
void ShowUIHelper::Start()
{
	StartUIIn();
}

bool ShowUIHelper::UpdateCheckFinish()
{
	if (State == END)
	{
		return true;
	}
	else if (State == START_WAIT)
	{
		StartUIWait();
	}
	else if (State == START_OUT)
	{
		StartUIOut();
	}
	return false;
}

void ShowUIHelper::StartUIIn()
{
	//set state
	State = UI_IN;

	//set anime "ui in"
	Animator* animator = new Animator(UIObject);
	animator->AddEndEvent([this]()
		{
			State = State::START_WAIT;
		});

	AniDesc_Vec3Hermite desc;
	ComputeHermiteVec3 computeFunc;
	desc.Duration = ShowUIDesc.DurationUIIn;
	desc.StartVec3 = ShowUIDesc.PositionUIInStart();
	desc.EndVec3 = ShowUIDesc.PositionUIWaitStart();
	desc.StartTangent = ShowUIDesc.TangentUIInStart;
	desc.EndTangent = ShowUIDesc.TangentUIInEnd;
	Anime_Position* posAnime = new Anime_Position(animator, desc, computeFunc);
}

void ShowUIHelper::StartUIWait()
{
	//set state
	State = UI_WAIT;

	//set anime "ui wait"
	Animator* animator = new Animator(UIObject);
	animator->AddEndEvent([this]()
		{
			State = State::START_OUT;
		});

	AniDesc_Vec3StartEnd desc;
	ComputeUniformVec3 computeFunc;
	desc.Duration = ShowUIDesc.DurationUIWait;
	desc.Start = ShowUIDesc.PositionUIWaitStart();
	desc.End = ShowUIDesc.PositionUIWaitEnd();
	Anime_Position* posAnime = new Anime_Position(animator, desc, computeFunc);
}

void ShowUIHelper::StartUIOut()
{
	//set state
	State = UI_OUT;

	//set anime "ui out"
	Animator* animator = new Animator(UIObject);
	animator->AddEndEvent([this]()
		{
			State = State::END;
		});

	AniDesc_Vec3Hermite desc;
	ComputeHermiteVec3 computeFunc;
	desc.Duration = ShowUIDesc.DurationUIOut;
	desc.StartVec3 = ShowUIDesc.PositionUIWaitEnd();
	desc.EndVec3 = ShowUIDesc.PositionUIOutEnd();
	desc.StartTangent = ShowUIDesc.TangentUIOutStart;
	desc.EndTangent = ShowUIDesc.TangentUIOutEnd;
	Anime_Position* posAnime = new Anime_Position(animator, desc, computeFunc);
}
#pragma endregion

#pragma region ========== mark ui anime helper ==========
void ShowMarkHelper::StartShowUI()
{
	State = UI_ANIME;

	if (UIObject != nullptr)
	{
		Animator* animator = new Animator(UIObject);
		animator->AddEndEvent([this]() {	State = State::END;	});

		AniDesc_Vec3Hermite posDesc;
		ComputeHermiteVec3 posComputeFunc;
		posDesc.Duration = Desc.ShowAniDesc.Duration;
		posDesc.StartVec3 = Desc.ShowAniDesc.StartPosition;
		posDesc.EndVec3 = Desc.ShowAniDesc.EndPosition;
		posDesc.StartTangent = Desc.ShowAniDesc.StartTangent;
		posDesc.EndTangent = Desc.ShowAniDesc.EndTangent;
		new Anime_Position(animator, posDesc, posComputeFunc);

		AniDesc_Vec3StartEnd sclDesc;
		ComputeUniformVec3 sclComputeFunc;
		sclDesc.Duration = Desc.ShowAniDesc.Duration;
		sclDesc.Start = Desc.ShowAniDesc.StartScale;
		sclDesc.End = Desc.ShowAniDesc.EndScale;
		new Anime_Scale(animator, sclDesc, sclComputeFunc);
	}
}

void ShowMarkHelper::StartClearUI()
{
	State = UI_ANIME;

	if (UIObject)
	{
		Animator* animator = new Animator(UIObject);
		animator->AddEndEvent([this]() {	State = State::END;	});

		AniDesc_Vec3Hermite posDesc;
		ComputeHermiteVec3 posComputeFunc;
		posDesc.Duration = Desc.ClearAniDesc.Duration;
		posDesc.StartVec3 = Desc.ClearAniDesc.StartPosition;
		posDesc.EndVec3 = Desc.ClearAniDesc.EndPosition;
		posDesc.StartTangent = Desc.ClearAniDesc.StartTangent;
		posDesc.EndTangent = Desc.ClearAniDesc.EndTangent;
		new Anime_Position(animator, posDesc, posComputeFunc);

		AniDesc_Vec3StartEnd sclDesc;
		ComputeUniformVec3 sclComputeFunc;
		sclDesc.Duration = Desc.ClearAniDesc.Duration;
		sclDesc.Start = Desc.ClearAniDesc.StartScale;
		sclDesc.End = Desc.ClearAniDesc.EndScale;
		new Anime_Scale(animator, sclDesc, sclComputeFunc);
	}
}
#pragma endregion

