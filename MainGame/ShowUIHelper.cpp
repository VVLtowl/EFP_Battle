#include "main.h"
#include "TransformAnime.h"
#include "ShowUIHelper.h"

#include "imgui/imgui.h"

#pragma region ========== step ui anime helper ==========
void ShowUIHelper::Start()
{
	StartUIIn();
}

bool ShowUIHelper::UpdateCheckFinish()
{
	if (m_State == END)
	{
		return true;
	}
	else if (m_State == START_WAIT)
	{
		StartUIWait();
	}
	else if (m_State == UI_WAIT)
	{
		UpdateWait();
	}
	else if (m_State == START_OUT)
	{
		StartUIOut();
	}
	return false;
}

void ShowUIHelper::StartUIIn()
{
	//set state
	m_State = UI_IN;

	//set anime "ui in"
	Animator* animator = new Animator(m_UIObject);
	animator->AddEndEvent([this]()
		{
			m_State = State::START_WAIT;
		});

	AniDesc_Vec3Hermite desc;
	ComputeHermiteVec3 computeFunc;
	desc.Duration = m_ShowUIDesc.DurationUIIn;
	desc.StartVec3 = m_ShowUIDesc.PositionUIInStart();
	desc.EndVec3 = m_ShowUIDesc.PositionUIWaitStart();
	desc.StartTangent = m_ShowUIDesc.TangentUIInStart;
	desc.EndTangent = m_ShowUIDesc.TangentUIInEnd;
	Anime_Position* posAnime = new Anime_Position(animator, desc, computeFunc);
}

void ShowUIHelper::StartUIWait()
{
	//set state
	m_State = UI_WAIT;

	//set anime "ui wait"
	Animator* animator = new Animator(m_UIObject);
	animator->AddEndEvent([this]()
		{
			m_State = State::START_OUT;
		});

	AniDesc_Vec3StartEnd desc;
	ComputeUniformVec3 computeFunc;
	desc.Duration = m_ShowUIDesc.DurationUIWait;
	desc.Start = m_ShowUIDesc.PositionUIWaitStart();
	desc.End = m_ShowUIDesc.PositionUIWaitEnd();
	Anime_Position* posAnime = new Anime_Position(animator, desc, computeFunc);
}

void ShowUIHelper::StartUIOut()
{
	//set state
	m_State = UI_OUT;

	//set anime "ui out"
	Animator* animator = new Animator(m_UIObject);
	animator->AddEndEvent([this]()
		{
			m_State = State::END;
		});

	AniDesc_Vec3Hermite desc;
	ComputeHermiteVec3 computeFunc;
	desc.Duration = m_ShowUIDesc.DurationUIOut;
	desc.StartVec3 = m_ShowUIDesc.PositionUIWaitEnd();
	desc.EndVec3 = m_ShowUIDesc.PositionUIOutEnd();
	desc.StartTangent = m_ShowUIDesc.TangentUIOutStart;
	desc.EndTangent = m_ShowUIDesc.TangentUIOutEnd;
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

#pragma region ========== game over ui anime helper ==========
void ShowGameOverUIHelper::StartUIWait()
{
	//set state
	m_State = UI_WAIT;

	//reset finish flag
	m_Finish = false;

	//set anime "ui wait"
	Animator* animator = new Animator(m_UIObject);
	//wahen anime over, make check finish panel 
	animator->AddEndEvent([this]()
		{
			DebugInfo::TestBlocks.emplace(TESTBLOCKID_CLIENT_CHECKGAMEOVERFINISH,
				[this]()
				{
					//set up piecec info panel pos
					float width = SCREEN_WIDTH / 5;
					float height = SCREEN_HEIGHT / 10;
					float posX = SCREEN_WIDTH_HALF - width / 2.0f;
					float posY = SCREEN_HEIGHT - height * 2.5f;
					ImGui::SetNextWindowPos(ImVec2(posX,posY ));
					ImGui::SetNextWindowSize(ImVec2(width, height));

					//begin draw piece info panel
					std::string title = "Check Game Over Finish";
					ImGui::Begin(title.c_str());
					{
						//finish button
						if (ImGui::Button("back to wait room"))
						{
							m_Finish = true;
						}
					}
					ImGui::End();
				});
		});

	AniDesc_Vec3StartEnd desc;
	ComputeUniformVec3 computeFunc;
	desc.Duration = m_ShowUIDesc.DurationUIWait;
	desc.Start = m_ShowUIDesc.PositionUIWaitStart();
	desc.End = m_ShowUIDesc.PositionUIWaitEnd();
	Anime_Position* posAnime = new Anime_Position(animator, desc, computeFunc);
}

void ShowGameOverUIHelper::UpdateWait()
{
	if (m_Finish)
	{
		//set state
		m_State = START_OUT;

		//close check panel
		DebugInfo::TestBlocks.erase(TESTBLOCKID_CLIENT_CHECKGAMEOVERFINISH);
	}
}
#pragma endregion

