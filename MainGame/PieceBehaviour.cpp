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
#include "LookAt.h"
#include "Camera.h"

#include "RPS.h"

#include "UIActSelection.h"
#include "UIThinkMark.h"
#include "UIHand.h"
#include "ButtonsPanel.h"
#include "ButtonObject.h"

const float DURATION_SHOWHAND = 20;
const float DURATION_CHECKACTPOINT_LEAST = 80;
const float DURATION_CHECKACTPOINT_EFFECTFLY = DURATION_CHECKACTPOINT_LEAST * 0.7f;
const float DURATION_SHOWBUTTON = 20; 
const float DURATION_HIDEBUTTON = 20;
const float DURATION_MOVE = 30; 
const float DURATION_ESCAPE = 30;
const float DURATION_CAUGHT = 30;
#pragma region ========== show entry ==========

void PieceShowEntry::Start()
{
	//init anime
	{
		GameObject* obj = m_Piece->m_PieceObject;
		D3DXVECTOR3 startSquarePos = m_Piece->m_FootOnSquare->
			m_SquareObject->GetTransform()->GetWorldPosition();
		D3DXVECTOR3 startPos = startSquarePos + D3DXVECTOR3(0, 10, 0);
		D3DXVECTOR3 endPos = startSquarePos + OFFSET_PIECE_ON_SQUARE;

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

#pragma region ========== show hand result ==========

void PieceShowHand::Start()
{
	//init val
	{
		FinishHandCount = 0;
		TargetHandCount = m_Piece->m_Hands.size();
	}

	//init anime
	{
		Transform3D* followTrans = m_Piece->m_PieceObject->
			GetUITransform(PieceObject::UITID::PAWN_UP_LEFT)->GetTransform();
		int count = -1;
		for (auto handType : m_Piece->m_Hands)
		{
			count++;//count: 0~size-1
			UIHand* uiHand = new UIHand();
			m_Piece->m_UIShowHands.emplace_back(uiHand);
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
					desc.Duration = DURATION_SHOWHAND;
					desc.StartVec3 = V3_ZERO;
					desc.EndVec3 = V3_ONE;
					desc.StartTangent = { 20,20,0 };
					desc.EndTangent = { -20,-20,0 };
					ComputeHermiteVec3 computeFunc;
					new Anime_Scale(animator, desc, computeFunc);
				}
				//position anime
				{
					AniDesc_Vec3Hermite desc;
					desc.Duration = DURATION_SHOWHAND;
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

#pragma region ========== show button panel ==========
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
			const float duration = DURATION_SHOWBUTTON;
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

#pragma region ========== show button panel button square ==========

void PieceShowButtonsPanel_ButtonSquare::Start()
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
			//set anime to all button objects
			for (auto btnObj : *buttonObjs)
			{
				ButtonInputMove* btnSquare = (ButtonInputMove*)btnObj;

				//create once animator for button object
				Animator* animator = new Animator(btnSquare);
				animator->AddEndEvent([this]()
					{
						FinishCount++;
					});

				//add scale anime
				AniDesc_Vec3StartEnd sclAniDesc;
				sclAniDesc.Duration = DURATION_SHOWBUTTON;
				sclAniDesc.Start = V3_ZERO;
				sclAniDesc.End = V3_ONE;
				ComputeSlowStartSlowEndVec3 sclCompute;
				new Anime_Scale(animator, sclAniDesc, sclCompute);

				//add position anime
				AniDesc_Vec3Hermite posAniDesc;
				posAniDesc.Duration = DURATION_SHOWBUTTON;
				posAniDesc.StartVec3 = m_Piece->m_FootOnSquare->m_SquareObject->
					GetTransform()->GetWorldPosition();
				posAniDesc.EndVec3 =
					btnSquare->m_TargetSquare->m_SquareObject->
					GetTransform()->GetWorldPosition() +
					OFFSET_PIECE_ON_SQUARE;
				posAniDesc.StartTangent = D3DXVECTOR3(0, 0, 0);
				posAniDesc.EndTangent = D3DXVECTOR3(0, 0, 0);
				ComputeHermiteVec3 posCompute;
				new Anime_Position(animator, posAniDesc, posCompute);
			}
		}
	}
}

void PieceShowButtonsPanel_ButtonSquare::Update()
{
	//check finish
	if (FinishCount >= TargetCount)
	{
		NextState();
	}
}

#pragma endregion

#pragma region ========== hide button panel button square ==========

void PieceHideButtonsPanel_ButtonSquare::Start()
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
			//set anime to all button objects
			for (auto btnObj : *buttonObjs)
			{
				ButtonInputMove* btnSquare = (ButtonInputMove*)btnObj;

				//create once animator for button object
				Animator* animator = new Animator(btnSquare);
				animator->AddEndEvent([this]()
					{
						FinishCount++;
					});

				//add scale anime
				AniDesc_Vec3StartEnd sclAniDesc;
				sclAniDesc.Duration = DURATION_HIDEBUTTON;
				sclAniDesc.Start = V3_ONE;
				sclAniDesc.End = V3_ONE;
				ComputeSlowStartSlowEndVec3 sclCompute;
				new Anime_Scale(animator, sclAniDesc, sclCompute);

				//add position anime
				AniDesc_Vec3StartEnd posAniDesc;
				posAniDesc.Duration = DURATION_HIDEBUTTON;
				posAniDesc.Start = 
					btnSquare->GetTransform()->GetWorldPosition();
				posAniDesc.End = m_TargetPos;
				ComputeSlowLerpVec3 posCompute;
				new Anime_Position(animator, posAniDesc, posCompute);
			}
		}
	}
}

void PieceHideButtonsPanel_ButtonSquare::Update()
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
		m_FrameCount = 0;
		m_ActpointChangeFrameCount = DURATION_CHECKACTPOINT_EFFECTFLY * 0.6f;
	}

	//make camera anime
	m_Piece->StartCameraLookAtThis(25, 
		[this]()
		{
			//get rival list
			std::list<Piece*>* rivals = nullptr;
			{
				if (m_Piece->m_Camp == CampType::BAD)
				{
					rivals = &GameManager::m_Goods;
				}
				else if (m_Piece->m_Camp == CampType::GOOD)
				{
					rivals = &GameManager::m_Bads;
				}
			}
			//save win effect to list for show anime
			std::list<WinEffectDesc> winEffectDescs;
			{
				for (auto selfHand : m_Piece->m_Hands)
				{
					for (auto rival : *rivals)
					{
						for (auto rivalHand : rival->m_Hands)
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
								winEffectDescs.emplace_back(desc);
							}
						}
					}
				}
			}

			//start effect anime
			{
				for (auto& desc : winEffectDescs)
				{
					//create effect obj
					Emitter* effect = new Emitter();
					m_Effects.emplace_back(effect);

					//anime value
					D3DXVECTOR3 dir = desc.RivalHandPos - desc.SelfHandPos;
					D3DXVec3Normalize(&dir, &dir);

					//create anime
					Animator* animator = new Animator(effect);
					animator->AddEndEvent(
						[this, effect]()
						{
							effect->SetState(GameObject::DEAD);
							m_Effects.remove(effect);
						}
					);

					//add pos anime
					{
						AniDesc_Vec3Hermite posDesc;
						posDesc.Duration = DURATION_CHECKACTPOINT_EFFECTFLY;
						posDesc.StartVec3 = desc.SelfHandPos;
						posDesc.EndVec3 = desc.RivalHandPos;
						posDesc.StartTangent = D3DXVECTOR3(0, 5, 0) - dir * 3;
						posDesc.EndTangent = dir * 3;
						ComputeHermiteVec3 computeFunc;
						new Anime_Position(animator, posDesc, computeFunc);
					}
				}
			}

			//set camera look at piece
			//CameraManager::GetMainCamera()->m_CmrLookAt->m_TargetTransform =
				//m_Piece->m_PieceObject->GetUITransform(PieceObject::UITID::PAWN_CENTER)->GetTransform();

			m_CheckState = State::SHOW_EFFECT_FLY;
		});
	CameraManager::GetMainCamera()->m_IsOrtho = true;

	//create selected mark
	m_Piece->SetUISelect(true);
}

void PieceShowCheckActpoint::Update()
{
	switch (m_CheckState)
	{
	case State::WAIT_CAMERA_MOVE_FINISH:
	{
		//wait camera move to look at pos
		PrintUpdateState("wait camera move finish");
		break;
	}

	case State::SHOW_EFFECT_FLY:
	{
		PrintUpdateState("show effect fly");

		if (m_Effects.empty() &&
			m_FrameCount >= DURATION_CHECKACTPOINT_LEAST)
		{
			//next state
			m_CheckState = State::FINISH;

			break;
		}
		else
		{
			if (m_FrameCount >= m_ActpointChangeFrameCount)
			{
				//test
				if (m_TargetActpoint != 0)
				{
					m_Piece->m_ActPoint = m_TargetActpoint;
					m_Piece->m_UIShowActpoint->StartNumberChangeAnime(m_TargetActpoint);
				}
			}

			m_FrameCount += FRAME_STRIDE;
		}

		break;
	}

	case State::FINISH:
	{
		PrintUpdateState("finish");

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
	UIActPoint* actpointUI = m_Piece->m_UIShowActpoint;
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
	Animator* animator = new Animator(m_Piece->m_UIShowActpoint);
	animator->AddEndEvent(
		[this]()
		{
			m_Piece->m_UIShowActpoint->m_OwnUINumbers->SetNumber(TargetNum);
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
	Animator* animator = new Animator(m_Piece->m_UIShowActpoint);
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

#pragma region ========== show move ==========

void PieceShowMove::Reset(const D3DXVECTOR3& targetPos)
{
	//reset value
	m_Finish = false;

	//init anime
	{
		GameObject* obj = m_Piece->m_PieceObject;
		D3DXVECTOR3 startPos = obj->GetTransform()->GetWorldPosition();
		D3DXVECTOR3 endPos = targetPos;
		D3DXVECTOR3 dir = endPos - startPos;
		D3DXVec3Normalize(&dir, &dir);

		Animator* animator = new Animator(obj);
		animator->AddEndEvent([this]()
			{
				m_Finish = true;
			});

		//add pos anime
		{
			AniDesc_Vec3Hermite posDesc;
			posDesc.Duration = DURATION_MOVE;
			posDesc.StartVec3 = startPos;
			posDesc.EndVec3 = endPos;
			posDesc.StartTangent = D3DXVECTOR3(0, 5, 0);
			posDesc.EndTangent = D3DXVECTOR3(0, -5, 0);
			ComputeHermiteVec3 computeFunc;
			new Anime_Position(animator, posDesc, computeFunc);
		}
	}
}

void PieceShowMove::Update()
{
	if (m_Finish)
	{
		NextState();
	}
}

#pragma endregion

#pragma region ========== show caught ==========

void PieceShowCaught::Reset(const D3DXVECTOR3& targetPos)
{
	//reset value
	m_Finish = false;

	//init anime
	{
		//set common value for anime
		GameObject* obj = m_Piece->m_PieceObject;
		D3DXVECTOR3 startPos = obj->GetTransform()->GetWorldPosition();
		D3DXVECTOR3 endPos = targetPos;
		D3DXVECTOR3 dir = endPos - startPos;
		const float duration = DURATION_CAUGHT;
		D3DXVec3Normalize(&dir, &dir);

		//create and set up animator component
		Animator* animator = new Animator(obj);
		animator->AddEndEvent([this]()
			{
				m_Finish = true;
			});

		//add scale anime
		{
			AniDesc_Vec3Hermite sclPos;
			sclPos.Duration = duration;
			sclPos.StartVec3 = V3_ONE;
			sclPos.EndVec3 = V3_ONE;
			sclPos.StartTangent = D3DXVECTOR3(0, 5, 0);
			sclPos.EndTangent = D3DXVECTOR3(0, -2, 0);
			ComputeHermiteVec3 computeFunc;
			new Anime_Scale(animator, sclPos, computeFunc);
		}

		//add pos anime
		{
			AniDesc_Vec3Hermite posDesc;
			posDesc.Duration = duration;
			posDesc.StartVec3 = startPos;
			posDesc.EndVec3 = endPos;
			posDesc.StartTangent = D3DXVECTOR3(0, 5, 0);
			posDesc.EndTangent = D3DXVECTOR3(0, -5, 0);
			ComputeHermiteVec3 computeFunc;
			new Anime_Position(animator, posDesc, computeFunc);
		}
	}
}

void PieceShowCaught::Update()
{
	if (m_Finish)
	{
		NextState();
	}
}

#pragma endregion

#pragma region ========== show escape ==========

void PieceShowEscape::Reset(const D3DXVECTOR3& targetPos)
{
	//reset value
	m_Finish = false;

	//init anime
	{
		//set common value for anime
		GameObject* obj = m_Piece->m_PieceObject;
		D3DXVECTOR3 startPos = obj->GetTransform()->GetWorldPosition();
		D3DXVECTOR3 endPos = targetPos;
		D3DXVECTOR3 dir = endPos - startPos;
		const float duration = DURATION_ESCAPE;
		D3DXVec3Normalize(&dir, &dir);

		//create and set up animator component
		Animator* animator = new Animator(obj);
		animator->AddEndEvent([this]()
			{
				m_Finish = true;
			});

		//add scale anime
		{
			AniDesc_Vec3Hermite sclPos;
			sclPos.Duration = duration;
			sclPos.StartVec3 = V3_ONE;
			sclPos.EndVec3 = V3_ONE;
			sclPos.StartTangent = D3DXVECTOR3(0, 5, 0);
			sclPos.EndTangent = D3DXVECTOR3(0, -2, 0);
			ComputeHermiteVec3 computeFunc;
			new Anime_Scale(animator, sclPos, computeFunc);
		}

		//add pos anime
		{
			AniDesc_Vec3Hermite posDesc;
			posDesc.Duration = duration;
			posDesc.StartVec3 = startPos;
			posDesc.EndVec3 = endPos;
			posDesc.StartTangent = D3DXVECTOR3(0, 5, 0);
			posDesc.EndTangent = D3DXVECTOR3(0, -5, 0);
			ComputeHermiteVec3 computeFunc;
			new Anime_Position(animator, posDesc, computeFunc);
		}
	}
}

void PieceShowEscape::Update()
{
	if (m_Finish)
	{
		NextState();
	}
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
								piece->m_ID,
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
			show->SetEndEvent("StartInputAfterAnime InputHand",
				[this]() 
				{
					//start input after anime
					this->InputState = State::INPUT;

					//active buttons after anime
					m_Piece->m_ButtonsPanel->
						m_ButtonGroup->SetState(Component::ACTIVE);
				});
			m_Piece->JumpToBH(show);
		}

		//set up now operate piece's selected mark anime
		{
			//test
			if (m_Piece->m_SelectedMark)
			{
				Animator* animator = new Animator(m_Piece->m_SelectedMark);
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

		//set camera look at this
		m_Piece->StartCameraLookAtThis(20);

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


#pragma endregion

#pragma region ========== common input ==========

void PieceInput::Update()
{
	switch (InputState)
	{
	case State::START_SHOW_UI:
	{
		PrintState("start show ui");
		m_OnStartShowUI();

		//show select mark and start anime
		m_Piece->SetUISelect(true);
		m_Piece->m_SelectedMark->SetHighlightAnime(true);

		//next state
		InputState = State::WAIT_SHOW_UI_FINISH;
		PrintState("start show ui ok");
		break;
	}
	case State::WAIT_SHOW_UI_FINISH:
	{
		//wait button panel finish showing anime
		PrintUpdateState("show ui");
		break;
	}

	case State::INPUT:
	{
		//wait player click button
		PrintUpdateState("input");
		break;
	}

	case State::START_HIDE_UI:
	{
		PrintState("start hide ui");
		m_OnStartHideUI();

		//clear select mark
		m_Piece->SetUISelect(false);

		//next state
		InputState = State::WAIT_HIDE_UI_FINISH;
		PrintUpdateState("start hide ui ok");
		break;
	}
	case State::WAIT_HIDE_UI_FINISH:
	{
		//wait button panel finish hiding anime
		PrintUpdateState("hide ui");
		break;
	}

	case State::WAIT_TO_CONTINUE:
	{
		//wait server judgement command or wait concrete act
		PrintUpdateState("wait to continue");
		break;
	}
	case State::FINISH:
	{
		//finish
		PrintState("input finish");
		NextState();
		PrintState("input finish ok");
		break;
	}
	}
}

void PieceInput::Reset(std::function<void()> showUI, std::function<void()> hideUI)
{
	//reset value
	//m_State = Behaviour::State::START;//tips: please use MainUpdate to ResetState
	InputState = State::START_SHOW_UI;

	//set up show ui event and hide ui event
	m_OnStartShowUI = showUI;
	m_OnStartHideUI = hideUI;
}

#pragma endregion



#pragma region ========== calculate actpoint ==========

void CalculateActpoint_Normal::Start()
{
	Piece* self = m_Piece;

	//check rivals
	std::list<Piece*>* rivals = nullptr;
	if (self->m_Camp == CampType::GOOD)
	{
		rivals = &GameManager::m_Bads;
	}
	else if (self->m_Camp == CampType::BAD)
	{
		rivals = &GameManager::m_Goods;
	}

	//self's every hand battle with every rival's every hand 
	for (auto selfHand : self->m_Hands)
	{
		for (auto rival : *rivals)
		{
			for (auto rivalHand : rival->m_Hands)
			{
				if (RPS::Battle(selfHand, rivalHand) == ResultType::WIN)
				{
					//todo 
					//insteal by BH
					self->m_ActPoint++;
				}
			}
		}
	}
}

#pragma endregion

#pragma region ========== clear actpoint ==========

void PieceClearActPoint::Start()
{
	m_Piece->m_ActPoint = 0;
}

#pragma endregion

#pragma region ========== clear hand ==========
void PieceClearHand::Start()
{
	m_Piece->m_Hands.clear();
}
#pragma endregion

#pragma region ========== calculate can go square ==========

void CalculateSquare_Normal::Start()
{
	m_CanGoSquares.clear();
	for (auto square : m_Piece->m_FootOnSquare->m_Neighbors)
	{
		if (square != nullptr)
		{
			if (square->CheckEnter(m_Piece) != Square::EnterResult::CAN_NOT_ENTER)
			{
				m_CanGoSquares.emplace_back(square);
			}
		}
	}
}

#pragma endregion

