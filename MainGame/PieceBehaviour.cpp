#include "main.h"

#include "Square.h"
#include "Board.h"
#include "Character.h"
#include "Piece.h"
#include "Player.h"
#include "Judgement.h"
#include "Client.h"
#include "NetworkManager.h"

#include "PieceBehaviour.h"

#include "TransformAnime.h"
#include "WorldToScreen.h"
#include "MyButtonGroup.h"
#include "Button.h"

#include "RPS.h"

#include "UIActSelection.h"
#include "UIThinkMark.h"
#include "UIHand.h"
#include "ButtonsPanel.h"
#include "ButtonObject.h"

#pragma region ========== show entry ==========

void PieceShowEntry::Start()
{
	//init anime
	{
		GameObject* obj = m_Piece->m_PieceObject;
		D3DXVECTOR3 startSquarePos = m_Piece->FootOnSquare->
			TargetSquareObject->GetTransform()->GetWorldPosition();
		D3DXVECTOR3 startPos = startSquarePos + D3DXVECTOR3(0, 10, 0);
		D3DXVECTOR3 endPos = startSquarePos + D3DXVECTOR3(0, 0.25f, 0);

		Animator* animator = new Animator(obj);
		animator->AddEndEvent([this]() { Finish = true; });
		AniDesc_Vec3StartEnd desc;
		desc.Duration = 40;
		desc.Start = startPos;
		desc.End = endPos;
		ComputeSlowStartSlowEndVec3 computeFunc;
		Anime_Position* anime = new Anime_Position(animator, desc, computeFunc);
	}
}

void PieceShowEntry::Update()
{
	if (Finish)
		//finish show entry anime 
	{
		NextState();
	}
}

#pragma endregion

#pragma region ========== show finish mark ==========

void PieceShowFinishMark::Start()
{
}

void PieceShowFinishMark::Update()
{
}

#pragma endregion

#pragma region ========== show hand ==========

void PieceShowHand::Start()
{
	//init val
	{
		FinishHandCount = 0;
		TargetHandCount = m_Piece->Hands.size();
	}

	//init anime
	{
		Transform3D* followTrans = m_Piece->m_PieceObject->
			GetUITransform(PieceObject::UITID::PAWN_UP_LEFT)->GetTransform();
		int count = -1;
		for (auto handType : m_Piece->Hands)
		{
			count++;//count: 0~size-1
			UIHand* uiHand = new UIHand();
			m_Piece->UIShowHands.emplace_back(uiHand);
			uiHand->SetHandType(handType);
			uiHand->GetTransform()->SetPosition(0, 10000, 0);
			uiHand->FollowWorldObject->SetTargetTransform(followTrans);

			//values for anime
			D3DXVECTOR3 startPos = D3DXVECTOR3(0, -200, 0);
			D3DXVECTOR3 endPos = V3_ZERO;
				//D3DXVECTOR3(0, -UI_HAND_SIZE.y * 1.1f * count, 0);

			//set up anime
			{
				Animator* animator = new Animator(uiHand);
				animator->AddEndEvent([this]() { FinishHandCount++; });
				//scale anime
				{
					AniDesc_Vec3Hermite desc;
					desc.Duration = 20;
					desc.StartVec3 = V3_ZERO;
					desc.EndVec3 = V3_ONE;
					desc.StartTangent = { 40,40,0 };
					desc.EndTangent = { 40,40,0 };
					ComputeHermiteVec3 computeFunc;
					new Anime_Scale(animator, desc, computeFunc);
				}
				//position anime
				{
					AniDesc_Vec3Hermite desc;
					desc.Duration = 20;
					desc.StartVec3 = startPos;
					desc.EndVec3 = endPos;
					desc.StartTangent = { 0,-100,0 };
					desc.EndTangent = { 0,-100,0 };
					ComputeHermiteVec3 computeFunc;
					new Anime_Position(animator, desc, computeFunc);
				}
			}
			
		}
	}
}

void PieceShowHand::Update()
{
	if (FinishHandCount >= TargetHandCount)
	{
		NextState();
	}
}

#pragma endregion

#pragma region ========== show button hand panel ==========
void PieceShowButtonsPanel::Start()
{
	//test init show button anime
	{
		//create anime
		std::list<ButtonObject*>* buttonObjs =
			m_Piece->m_ButtonsPanel->m_ButtonGroup->GetButtonObjs();

		//set target count and reset finish count
		TargetCount = buttonObjs->size();
		FinishCount = 0;

		//init anime
		{
			//values for scale anime and position circle rotate anime  
			const float num = buttonObjs->size();
			const float radius = PIECE_IMAGE_SIZE.y / 2.0f;
			const float startAngle = PI_HALF;
			const float intervalAngle = PI * 2.0f / num;
			const float duration = 40;
			const D3DXVECTOR3 center = V3_ZERO;

			//set up common scale anime desc
			AniDesc_Vec3StartEnd sclAniDesc;
			sclAniDesc.Duration = duration;
			sclAniDesc.Start = V3_ZERO;
			sclAniDesc.End = V3_ONE;

			//set up common position anime desc
			AniDesc_PositionCircleRotate posAniDesc;
			posAniDesc.Duration = duration;
			posAniDesc.Center = center;
			posAniDesc.Radius = radius;
			posAniDesc.StartRadian = startAngle;

			//set anime to all button objects
			float count = 0;
			for (auto btnObj : *buttonObjs)
			{
				//create once animator for button object
				Animator* animator = new Animator(btnObj);
				animator->AddEndEvent([this]()
					{
						FinishCount++;
					});

				//add scale anime
				ComputeSlowStartSlowEndVec3 sclCompute;
				Anime_Scale* sclAni = new Anime_Scale(animator, sclAniDesc, sclCompute);

				//add position anime
				posAniDesc.EndRadian = startAngle + intervalAngle * count;
				ComputePositionCircleRotate posCompute;
				Anime_Position* posAni = new Anime_Position(animator, posAniDesc, posCompute);

				count++;
			}
		}
	}
}

void PieceShowButtonsPanel::Update()
{
	//check finish
	if (FinishCount >= TargetCount)
	{
		NextState();
	}
}
#pragma endregion

#pragma region ========== show check act point ==========

void PieceShowCheckActpoint::Start()
{
	//reset frame count
	{
		FrameCount = 0;
		LeastFrameCount = 80;
		EffectDuration = LeastFrameCount - 40;
		ActpointChangeFrameCount = EffectDuration * 0.6f;
	}

	//get rival list
	std::list<Piece*>* rivals = nullptr;
	{
		if (m_Piece->Camp == CampType::BAD)
		{
			rivals = &Client::Instance()->Goods;
		}
		else if (m_Piece->Camp == CampType::GOOD)
		{
			rivals = &Client::Instance()->Bads;
		}
	}

	//save win effect to list for show anime
	std::list<WinEffectDesc> WinEffectDescs;
	TargetActpoint = m_Piece->ActPoint;
	{
		for (auto selfHand : m_Piece->Hands)
		{
			for (auto rival : *rivals)
			{
				for (auto rivalHand : rival->Hands)
				{
					if (RPS::Battle(selfHand, rivalHand) == ResultType::WIN)
					{
						D3DXVECTOR3 selfPos = m_Piece->m_PieceObject->
							GetUITransform(PieceObject::UITID::PAWN_UP_LEFT)->
							GetTransform()->GetWorldPosition();
						D3DXVECTOR3 rivalPos = rival->m_PieceObject->
							GetUITransform(PieceObject::UITID::PAWN_UP_LEFT)->
							GetTransform()->GetWorldPosition();

						WinEffectDesc desc;
						desc.SelfHandPos = selfPos;
						desc.RivalHandPos = rivalPos;
						WinEffectDescs.emplace_back(desc);

						TargetActpoint++;
					}
				}
			}
		}
	}

	//init effect anime
	{
		for (auto& desc : WinEffectDescs)
		{
			//create effect obj
			Emitter* effect = new Emitter();
			Effects.emplace_back(effect); 

			//anime value
			D3DXVECTOR3 dir = desc.RivalHandPos - desc.SelfHandPos;
			D3DXVec3Normalize(&dir, &dir);

			//create anime
			Animator* animator = new Animator(effect);
			animator->AddEndEvent(
				[this, effect]()
				{
					effect->SetState(GameObject::DEAD);
					Effects.remove(effect);
				}
			);

			//add pos anime
			{
				AniDesc_Vec3Hermite posDesc;
				posDesc.Duration = EffectDuration;
				posDesc.StartVec3 = desc.SelfHandPos;
				posDesc.EndVec3 = desc.RivalHandPos;
				posDesc.StartTangent = D3DXVECTOR3(0, 5, 0) - dir * 3;
				posDesc.EndTangent = dir * 3;
				ComputeHermiteVec3 computeFunc;
				new Anime_Position(animator, posDesc, computeFunc);
			}
		}
	}

	//create selected mark
	m_Piece->SetUISelect(true);
}

void PieceShowCheckActpoint::Update()
{
	switch (CheckState)
	{
	case State::SHOW_EFFECT_FLY:
	{
		if (Effects.empty() &&
			FrameCount >= LeastFrameCount)
		{
			{
				//next state
				//CheckState = State::WAIT_SHOW_ACTPOINT_CHANGE;

				//jump to show actpoint change
				{
					/*PieceShowActpointChange* showActpointChange =
						new PieceShowActpointChange(m_Piece, TargetActpoint);
					JumpTo(showActpointChange);*/
				}
			}

			//next state
			CheckState = State::FINISH;

			break;
		}
		else
		{
			if (FrameCount >= ActpointChangeFrameCount)
			{
				//test
				if (TargetActpoint != 0)
				{
					m_Piece->ActPoint = TargetActpoint;
					m_Piece->UIShowActpoint->StartNumberChangeAnime(TargetActpoint);

					TargetActpoint = 0;
				}
			}

			FrameCount += FRAME_STRIDE;
		}

		break;
	}
	case State::WAIT_SHOW_ACTPOINT_CHANGE:
	{
		//next state
		CheckState = State::FINISH;
		break;
	}
	case State::FINISH:
	{
		//clear select mark
		m_Piece->SetUISelect(false);


		//finish
		NextState();
		break;
	}
	}
}

#pragma endregion

#pragma region ========== show actpoint change ==========

void PieceShowActpointChange::Start()
{
	//init anime
	UIActPoint* actpointUI = m_Piece->UIShowActpoint;
	if(actpointUI)
	{
		StartBigger();
	}
	else
	{
		Finish = true;
	}
}

void PieceShowActpointChange::Update()
{
	if (Finish)
	{
		NextState();
	}
}

void PieceShowActpointChange::StartBigger()
{
	Animator* animator = new Animator(m_Piece->UIShowActpoint);
	animator->AddEndEvent(
		[this]()
		{
			m_Piece->UIShowActpoint->OwnUINumbers->SetNumber(TargetNum);
			StartBack();
		}
	);
	AniDesc_Vec3StartEnd desc;
	desc.LoopCount = 1;
	desc.Duration = 20;
	desc.Start = V3_ONE;
	desc.End = V3_ONE * 1.4f;
	ComputeSlowLerpVec3 computefunc;
	new Anime_Scale(animator, desc, computefunc);
}

void PieceShowActpointChange::StartBack()
{
	Animator* animator = new Animator(m_Piece->UIShowActpoint);
	animator->AddEndEvent(
		[this]()
		{
			Finish = true;
		}
	);
	AniDesc_Vec3StartEnd desc;
	desc.LoopCount = 1;
	desc.Duration = 20;
	desc.Start = V3_ONE * 1.4f;
	desc.End = V3_ONE;
	ComputeSlowLerpVec3 computefunc;
	new Anime_Scale(animator, desc, computefunc);
}

#pragma endregion

#pragma region ========== input hand ==========

void PieceInputHand::Start()
{
	InputState = State::START_SHOW_UI;
}

void PieceInputHand::Update()
{
	switch (InputState)
	{
	case State::START_SHOW_UI:
	{
		//set up button panel
		{
			//create button panel
			m_Piece->m_ButtonsPanel = new ButtonsPanel();
			ButtonsPanel* panel = m_Piece->m_ButtonsPanel;
			panel->SetParent(
				m_Piece->m_PieceObject->
				GetUITransform(PieceObject::UITID::PAWN_CENTER));

			//set up input finish event
			panel->m_ButtonGroup->OnFinish = [this, panel]()
			{
				this->InputState = PieceInputHand::State::WAIT_JUDGE_SET;
				panel->SetState(GameObject::DEAD);
			};

			//create button hand objects and add them to panel
			{
				ButtonHand* handRock = new ButtonHand(m_Piece);
				ButtonHand* handPaper = new ButtonHand(m_Piece);
				ButtonHand* handScissors = new ButtonHand(m_Piece);

				//set up buttons' texture
				handRock->SetHandType(HandType::ROCK);
				handPaper->SetHandType(HandType::PAPER);
				handScissors->SetHandType(HandType::SCISSORS);

				//add to ui control
				panel->m_ButtonGroup->AddButtonObject(handRock);
				panel->m_ButtonGroup->AddButtonObject(handPaper);
				panel->m_ButtonGroup->AddButtonObject(handScissors);

				for (auto btnObj : *panel->m_ButtonGroup->GetButtonObjs())
				{
					//hide at first
					btnObj->GetTransform()->SetPosition(0, 10000, 0);

					//set parent 
					btnObj->SetParent(panel);

					//add click event
					btnObj->GetButton()->AddClickEvent(
						[this, btnObj, panel]()
						{
							ButtonHand* btnHand = (ButtonHand*)btnObj;
							HandType handType = btnHand->Type;
							Piece* piece = btnHand->TargetPiece;
							//TargetPiece->Hands.emplace_back(handType);
							//start wait judgement check
							NetworkManager::Instance()->Piece_RequestSetHand(
								piece->ID,
								handType);

							//once decided hand, button group finish 
							panel->m_ButtonGroup->ChangeState(ButtonGroup::State::FINISH);
						}
					);
				}
			}
		}

		//start show button panel
		{
			//paused buttons before anime
			m_Piece->m_ButtonsPanel->
				m_ButtonGroup->SetState(Component::PAUSED);

			//jump to show bh
			PieceShowButtonsPanel* show = new PieceShowButtonsPanel(m_Piece);
			show->SetEndEvent([this]() 
				{
					//start input after anime
					this->InputState = State::INPUT;

					//active buttons after anime
					m_Piece->m_ButtonsPanel->
						m_ButtonGroup->SetState(Component::ACTIVE);
				});
			JumpTo(show);
		}

		//set up now operate piece's selected mark anime
		{
			//test
			if (m_Piece->SelectedMark)
			{
				Animator* animator = new Animator(m_Piece->SelectedMark);
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
				new Anime_Scale(animator, sclDesc, computeFunc);
			}
		}

		//next state
		InputState = State::WAIT_SHOW_UI_FINISH;
		break;
	}
	case State::WAIT_SHOW_UI_FINISH:
	{
		//wait button panel finish showing anime
		break;
	}
	case State::INPUT:
	{
		//wait player celect button
		break;
	}
	case State::WAIT_JUDGE_SET:
	{
		//wait request server judgement to check and command back
		break;
	}
	case State::FINISH:
	{
		//clear select mark
		m_Piece->SetUISelect(false);

		//finish
		NextState();
		break;
	}
	}
}

void PieceInputHand::End()
{
	//reset state
	InputState = State::START_SHOW_UI;
}

#pragma endregion

#pragma region ========== common input ==========

void PieceInput::Start()
{
	InputState = State::START_SHOW_UI;
}

void PieceInput::Update()
{
	switch (InputState)
	{
	case State::START_SHOW_UI:
	{
		m_OnStartShowUI();

		//show select mark and start anime
		m_Piece->SetUISelect(true);
		m_Piece->SelectedMark->SetHighlightAnime(true);

		//next state
		InputState = State::WAIT_SHOW_UI_FINISH;
		break;
	}
	case State::WAIT_SHOW_UI_FINISH:
	{
		//wait button panel finish showing anime
		break;
	}

	case State::INPUT:
	{
		//wait player click button
		break;
	}

	case State::START_HIDE_UI:
	{
		m_OnStartHideUI();

		//next state
		InputState = State::WAIT_HIDE_UI_FINISH;
		break;
	}
	case State::WAIT_HIDE_UI_FINISH:
	{
		//wait button panel finish hiding anime
		break;
	}

	case State::WAIT_TO_CONTINUE:
	{
		//wait server judgement command or wait concrete act
		break;
	}
	case State::FINISH:
	{
		//clear select mark
		m_Piece->SetUISelect(false);

		//finish
		NextState();
		break;
	}
	}
}

void PieceInput::End()
{
	//reset state
	InputState = State::START_SHOW_UI;
}

#pragma endregion

#pragma region ========== clear actpoint ==========
void PieceClearActPoint::Start()
{
	m_Piece->ActPoint = 0;
}
#pragma endregion

#pragma region ========== clear hand ==========
void PieceClearHand::Start()
{
	m_Piece->Hands.clear();
}
#pragma endregion
