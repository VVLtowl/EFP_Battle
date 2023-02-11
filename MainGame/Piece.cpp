#include "main.h"
#include "Character.h"
#include "Piece.h"

#include "Player.h"
#include "PieceBehaviour.h"

#include "UIThinkMark.h"
#include "UIHand.h"
#include "ButtonsPanel.h"
#include "ButtonObject.h"
#include "UIActSelection.h"

#include "Button.h"
#include "TransformAnime.h"
#include "WorldToScreen.h"

#include "NetworkManager.h"

Piece::Piece()
{
	BH_InputHand = new PieceInputHand(this);
	BH_Input = new PieceInput(this);
}

Piece::~Piece()
{
	OwnerPlayer = nullptr;
	FootOnSquare = nullptr;
	Hands.clear();

	if (m_PieceObject) { m_PieceObject->SetState(GameObject::DEAD); }
	delete OwnCharacter;

	if (FinishMark) { FinishMark->SetState(GameObject::DEAD); }
	if (ActMark) { ActMark->SetState(GameObject::DEAD); }
	if (ThinkMark) { ThinkMark->SetState(GameObject::DEAD); }
	for (auto hand : UIShowHands) { hand->SetState(GameObject::DEAD); };
	if (UIShowActpoint) { UIShowActpoint->SetState(GameObject::DEAD); }
	if (SelectedMark) { SelectedMark->SetState(GameObject::DEAD); }
}

void Piece::StartInputHand(std::function<void()> endEvent)
{
	BH_InputHand->SetEndEvent(endEvent);
	StartBH(BH_InputHand);
}

void Piece::AddHand(HandType handType)
{
	//add hand
	Hands.emplace_back(handType);
	DebugInfo::Print(
		"add [" +
		std::string(Hand::HandName(handType)) +
		"] to piece[" +
		std::to_string(ID) +
		"]");
}

void Piece::FinishSetHand(HandType handType)
{
	//add hand data
	AddHand(handType);

	//update mark
	{
		//delete think mark
		SetUIThink(false);

		//stop select mark's anime
		/*if (SelectedMark)
		{
			SelectedMark->GetComponent<Animator>()->
				SetState(Component::DEAD);
			SelectedMark->GetTransform()->SetScale(V3_ONE);
		}*/
		SetUISelect(false);

		//create finish mark
		SetUIFinish(true);
	}

	//check continue input behaviour
	if(BH_InputHand)
	{
		if (BH_InputHand->InputState == PieceInputHand::State::WAIT_JUDGE_SET)
		{
			//next state
			BH_InputHand->InputState = PieceInputHand::State::FINISH;
		}
	}
}

void Piece::StartInputSelectAct()
{
	//set up BH_Input show ui
	BH_Input->SetStartShowUIEvent([this]()
		{
			//set up button panel
			{
				//create button panel
				m_ButtonsPanel = new ButtonsPanel();
				m_ButtonsPanel->SetParent(
					m_PieceObject->GetUITransform(PieceObject::UITID::PAWN_CENTER));


				//create button hand objects and add them to panel
				{
					ButtonActSelection* actFinish = new ButtonActSelection(this);
					ButtonActSelection* actMove = new ButtonActSelection(this);

					//test
					//set up buttons' texture
					actFinish->m_UIActSelection->SetActTex(TEXID_UI_RPS);
					actMove->m_UIActSelection->SetActTex(TEXID_UI_MARK);

					//check can move
					if (ActPoint <= 0)
					{
						actMove->GetButton()->ChangeState(Button::State::DISABLE);
					}


					//add click event
					//BH_Input.HideUI -> BH_Input.HideUI end -> ButtonsPanel.ButtonGroup.OnFinish -> start concrete act
					{
						actFinish->GetButton()->AddClickEvent(
							[this]()
							{
								//set up input finish event
								m_ButtonsPanel->m_ButtonGroup->OnFinish = [this]()
								{
									//tell server judgement this piece has finished act
									NetworkManager::Instance()->Piece_RequestFinishAct(ID);
								};

								//next state
								BH_Input->InputState = PieceInput::State::START_HIDE_UI;
							}
						);

						actMove->GetButton()->AddClickEvent(
							[this]()
							{
								//set up input finish event
								m_ButtonsPanel->m_ButtonGroup->OnFinish = [this]()
								{
									//todo
									//start input move act
									//NetworkManager::Instance()->Piece_RequestMoveAct(m_Piece->ID);
								};

								//next state
								BH_Input->InputState = PieceInput::State::START_HIDE_UI;
							}
						);
					}

					//add buttons to UIControl
					m_ButtonsPanel->m_ButtonGroup->AddButtonObject(actFinish);
					m_ButtonsPanel->m_ButtonGroup->AddButtonObject(actMove);

					//set transform
					for (auto btnObj : *m_ButtonsPanel->m_ButtonGroup->GetButtonObjs())
					{
						//hide at first
						btnObj->GetTransform()->SetPosition(0, 10000, 0);

						//set parent 
						btnObj->SetParent(m_ButtonsPanel);
					}
				}
			}

			//start show button panel
			{
				//paused buttons before anime
				m_ButtonsPanel->m_ButtonGroup->SetState(Component::PAUSED);

				//set up showBH and jump to showBH
				PieceShowButtonsPanel* show = new PieceShowButtonsPanel(this);
				show->SetEndEvent([this]()
					{
						//start input after anime
						BH_Input->InputState = PieceInput::State::INPUT;

						//active buttons after anime
						m_ButtonsPanel->m_ButtonGroup->SetState(Component::ACTIVE);
					});
				BH_Input->JumpTo(show);
			}
		});

	//set up BH_Input hide ui
	BH_Input->SetStartHideUIEvent([this]()
		{
			//make clear panel anime
			Animator* animator = new Animator(m_ButtonsPanel);
			animator->AddEndEvent(
				[this]()
				{
					//finish event before dead
					m_ButtonsPanel->m_ButtonGroup->ChangeState(ButtonGroup::State::FINISH);
					m_ButtonsPanel->SetState(GameObject::DEAD);

					//set PieceInput to next state
					BH_Input->InputState = PieceInput::State::WAIT_TO_CONTINUE;
				}
			);
			AniDesc_Vec3Hermite sclDesc;
			{
				sclDesc.Duration = 10;
				sclDesc.StartVec3 = V3_ONE;
				sclDesc.EndVec3 = V3_ZERO;
				sclDesc.StartTangent = V3_ONE;
				sclDesc.EndTangent = -V3_ONE;
			}
			ComputeHermiteVec3 computeFunc;
			new Anime_Scale(animator, sclDesc, computeFunc);
		});

	//start to input select act
	StartBH(BH_Input);
}


void Piece::SetUISelect(bool show)
{
	if (show)
	{
		if (!SelectedMark)
		{
			UISelect* selectMark = new UISelect();
			selectMark->GetTransform()->SetRotation(PI / 2, 0, 0);
			GameObject* parent = m_PieceObject->
				GetUITransform(PieceObject::UITID::ROOT);
			D3DXVECTOR3 targetPos = parent->GetTransform()->GetWorldPosition();
			selectMark->GetTransform()->SetPosition(targetPos);

			SelectedMark = selectMark;
		}
	}
	else
	{
		if (SelectedMark)
		{
			SelectedMark->SetState(GameObject::DEAD);
			SelectedMark = nullptr;
		}
	}
}

void Piece::SetUIFinish(bool show)
{
	if (show)
	{
		if (!FinishMark)
		{
			UIFinishMark* finishMark = new UIFinishMark();
			finishMark->FollowWorldObject->SetTargetTransform(
				m_PieceObject->GetUITransform(PieceObject::UITID::PAWN_UP)->GetTransform());

			FinishMark = finishMark;
		}
	}
	else
	{
		if (FinishMark)
		{
			FinishMark->SetState(GameObject::DEAD);
			FinishMark = nullptr;
		}
	}
}

void Piece::SetUIThink(bool show)
{
	if (show)
	{
		if (!ThinkMark)
		{
			UIThinkMark* thinkMark = new UIThinkMark();
			thinkMark->FollowWorldObject->SetTargetTransform(
				m_PieceObject->GetUITransform(PieceObject::UITID::PAWN_UP)->GetTransform());

			ThinkMark = thinkMark;
		}
	}
	else
	{
		if (ThinkMark)
		{
			ThinkMark->SetState(GameObject::DEAD);
			ThinkMark = nullptr;
		}
	}
}

void Piece::SetUIHands(bool show)
{
	if (show)
	{
		//if (UIShowHands.empty())
		//{
		//	UIHand* hand = new UIHand();
		//	hand->FollowWorldObject->SetTargetTransform(
		//		m_PieceObject->GetUITransform(PieceObject::UITID::PAWN_UP)->GetTransform());

		//	//todo
		//}
	}
	else
	{
		if (!UIShowHands.empty())
		{
			for (auto obj : UIShowHands)
			{
				obj->SetState(GameObject::DEAD);
			}
			UIShowHands.clear();
		}
	}
}

void Piece::SetUIActpoint(bool show)
{
	if (show)
	{
		if (!UIShowActpoint)
		{
			UIActPoint* uiActpoint = new UIActPoint();
			uiActpoint->FollowWorldObject->SetTargetTransform(
				m_PieceObject->GetUITransform(PieceObject::UITID::PAWN_UP_RIGHT)->GetTransform());

			UIShowActpoint = uiActpoint;
		}
	}
	else
	{
		if (UIShowActpoint)
		{
			UIShowActpoint->SetState(GameObject::DEAD);
			UIShowActpoint = nullptr;
		}
	}
}