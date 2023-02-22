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
#include "UISquareSelection.h"

#include "Button.h"
#include "TransformAnime.h"
#include "WorldToScreen.h"
#include "LookAtCamera.h"
#include "LookAt.h"

#include "Board.h"
#include "NetworkManager.h"

Piece::Piece()
{
	BH_InputHand = new PieceInputHand(this);
	BH_Input = new PieceInput(this);
	BH_CalCanGoSquare = new CalculateSquare_Normal(this);
}

Piece::~Piece()
{
	m_OwnerPlayer = nullptr;
	m_FootOnSquare = nullptr;
	m_Hands.clear();

	if (m_PieceObject) 
	{ 
		m_PieceObject->SetState(GameObject::DEAD); 
	}
	//delete m_OwnCharacter;

	SetUIFinish(false);
	SetUIThink(false);
	SetUISelect(false);
	SetUIHands(false);
	SetUIActpoint(false);
}

void Piece::StartInputHand(std::function<void()> endEvent)
{
	BH_InputHand->SetEndEvent("",endEvent);
	StartBH(BH_InputHand);
}

void Piece::AddHand(HandType handType)
{
	//add hand
	m_Hands.emplace_back(handType);
	DebugInfo::Print(
		"add [" +
		std::string(Hand::HandName(handType)) +
		"] to piece[" +
		std::to_string(m_ID) +
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
	DebugInfo::Print("");
	DebugInfo::Print(
		"- start piece " +
		std::to_string(m_ID) +
		" select act...");

	BH_Input->Reset(
		//set up BH_Input show ui
		[this]()
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
					actFinish->m_UIActSelection->SetActTex(TEXID_UI_ACT_FINISH);
					actMove->m_UIActSelection->SetActTex(TEXID_UI_ACT_MOVE);

					//check can move
					if (m_ActPoint <= 0)
					{
						actMove->GetButton()->ChangeState(Button::State::DISABLE);
					}


					//add click event
					//BH_Input.HideUI -> BH_Input.HideUI end -> ButtonsPanel.ButtonGroup.OnFinish -> start concrete act
					{
						actFinish->GetButton()->AddClickEvent([this]()
							{
								//set up input finish event
								m_ButtonsPanel->m_ButtonGroup->OnFinish = [this]()
								{
									//delete self panel
									m_ButtonsPanel->SetState(GameObject::DEAD);
									m_ButtonsPanel = nullptr;

									//set PieceInput to next state
									BH_Input->InputState = PieceInput::State::FINISH;

									//tell server judgement this piece has finished act
									NetworkManager::Instance()->Piece_RequestFinishAct(m_ID);
								};

								//next state
								BH_Input->InputState = PieceInput::State::START_HIDE_UI;
							});

						actMove->GetButton()->AddClickEvent([this]()
							{
								//set up input finish event
								m_ButtonsPanel->m_ButtonGroup->OnFinish = [this]()
								{
									//delete now panel
									m_ButtonsPanel->SetState(GameObject::DEAD);
									m_ButtonsPanel = nullptr;

									//set PieceInput to next state
									BH_Input->InputState = PieceInput::State::FINISH;

									//start input move act
									BH_Input->MainUpdate();//finish last input
									StartInputMove();
								};

								//next state
								BH_Input->InputState = PieceInput::State::START_HIDE_UI;
							});
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

			//make camera anime
			StartCameraLookAtThis();

			//start show button panel
			{
				//paused buttons before anime
				m_ButtonsPanel->m_ButtonGroup->SetState(Component::PAUSED);

				//set up showBH and jump to showBH
				PieceShowButtonsPanel* show = new PieceShowButtonsPanel(this);
				show->SetEndEvent("StartInputAfterAnime", 
					[this]()
					{
						//start input after anime
						BH_Input->InputState = PieceInput::State::INPUT;

						//active buttons after anime
						m_ButtonsPanel->m_ButtonGroup->SetState(Component::ACTIVE);
					});
				
				//BH_Input->JumpTo(show);
				JumpToBH(show);
			}
		},
		//set up BH_Input hide ui
		[this]()
		{
			//paused buttons before anime
			m_ButtonsPanel->m_ButtonGroup->SetState(Component::PAUSED);

			//make clear panel anime
			Animator* animator = new Animator(m_ButtonsPanel);
			animator->AddEndEvent([this]()
				{
					//start finish event
					m_ButtonsPanel->m_ButtonGroup->ChangeState(ButtonGroup::State::FINISH);
				});
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

	BH_Input->SetEndEvent("DebugHintFinishSelectAct", 
		[this]()
		{
			DebugInfo::Print(
				"- finish piece " +
				std::to_string(m_ID) +
				" select act");
		});

	//start to input select act
	StartBH(BH_Input);
}

void Piece::StartInputMove()
{
	DebugInfo::Print("");
	DebugInfo::Print(
		"- start piece " +
		std::to_string(m_ID) +
		" input move...");

	BH_Input->Reset(
		//set up BH_Input show ui
		[this]()
		{
			//set up button panel
			{
				//create button panel
				m_ButtonsPanel = new ButtonsPanel();
				//tips: square button needn't look at camera
				m_ButtonsPanel->m_LookAtCmr->SetState(Component::DEAD);

				//create button square objects and add them to panel
				{
					//calculate can go square
					//tips: need JumpTo to set next bh back to inputBH
					//BH_Input->JumpTo(BH_CalCanGoSquare);
					//Execute();
					//BH_CalCanGoSquare->MainUpdate();
					JumpToBH(BH_CalCanGoSquare);
					Execute();

					//create and set up cancel button
					{
						//create button square
						ButtonMoveCancel* buttonCancel = new ButtonMoveCancel(this, m_FootOnSquare);
						buttonCancel->GetTransform()->SetRotation(PI_HALF, 0, 0);
						m_ButtonsPanel->m_ButtonGroup->AddButtonObject(buttonCancel);

						//hide at first
						buttonCancel->GetTransform()->SetPosition(0, 10000, 0);

						//set parent 
						buttonCancel->SetParent(m_ButtonsPanel);

						//add click event
						buttonCancel->GetButton()->AddClickEvent([this]()
							{
								//set up input finish event
								m_ButtonsPanel->m_ButtonGroup->OnFinish = [this]()
								{
									//delete panel
									m_ButtonsPanel->SetState(GameObject::DEAD);
									m_ButtonsPanel = nullptr;

									//set PieceInput to next state
									BH_Input->InputState = PieceInput::State::FINISH;
									BH_Input->MainUpdate();

									//back to select act
									StartInputSelectAct();
								};

								//inputBH's next state
								BH_Input->InputState = PieceInput::State::START_HIDE_UI;
								m_MoveToSquare = m_FootOnSquare;
							});
					}

					//create and set up square buttons
					std::list<Square*>* canGoSquares = BH_CalCanGoSquare->GetCanGoSquares();
					for (auto square : *canGoSquares)
					{
						//create button square
						ButtonSquareSelection* buttonSquare = new ButtonSquareSelection(this, square);
						buttonSquare->GetTransform()->SetRotation(PI_HALF, 0, 0);
						m_ButtonsPanel->m_ButtonGroup->AddButtonObject(buttonSquare);

						//hide at first
						buttonSquare->GetTransform()->SetPosition(0, 10000, 0);

						//set parent 
						buttonSquare->SetParent(m_ButtonsPanel);

						//add click event
						buttonSquare->GetButton()->AddClickEvent([this, square]()
							{
								//set up input finish event
								m_ButtonsPanel->m_ButtonGroup->OnFinish = [this, square]()
								{
									//delete panel
									m_ButtonsPanel->SetState(GameObject::DEAD);
									m_ButtonsPanel = nullptr;

									//set PieceInput to next state
									BH_Input->InputState = PieceInput::State::FINISH;
									BH_Input->MainUpdate();

									//request server to judge
									NetworkManager::Instance()->Piece_RequestMoveAct(m_ID, square->m_ID);
								};

								//inputBH's next state
								BH_Input->InputState = PieceInput::State::START_HIDE_UI;
								m_MoveToSquare = square;
							});
					}

				}
			}

			//make camera anime
			StartCameraLookAtThis();

			//start show button panel
			{
				//paused buttons before anime
				m_ButtonsPanel->m_ButtonGroup->SetState(Component::PAUSED);

				//set up showBH and jump to showBH
				PieceShowButtonsPanel_ButtonSquare* show =
					new PieceShowButtonsPanel_ButtonSquare(this);
				show->SetEndEvent("StartInputAfterAnime", 
					[this]()
					{
						//start input after anime
						BH_Input->InputState = PieceInput::State::INPUT;

						//active buttons after anime
						m_ButtonsPanel->m_ButtonGroup->SetState(Component::ACTIVE);
					});
				//BH_Input->JumpTo(show);
				JumpToBH(show);
			}
		},
		//set up BH_Input hide ui
		[this]()
		{
			//paused buttons before anime
			m_ButtonsPanel->m_ButtonGroup->SetState(Component::PAUSED);

			//set up showBH and jump to showBH
			const D3DXVECTOR3 targetPos =
				m_MoveToSquare->m_SquareObject->
				GetTransform()->GetWorldPosition() +
				D3DXVECTOR3(0, 0.1f, 0);
			PieceHideButtonsPanel_ButtonSquare* hide =
				new PieceHideButtonsPanel_ButtonSquare(this, targetPos);
			hide->SetEndEvent("StartButtonGroupOnFinishEvent", 
				[this]()
				{
					//start finish event
					m_ButtonsPanel->m_ButtonGroup->ChangeState(ButtonGroup::State::FINISH);
				});
			//BH_Input->JumpTo(hide);
			JumpToBH(hide);
		});

	BH_Input->SetEndEvent("DebugHintFinishPieceInputMove", 
		[this]()
		{
			DebugInfo::Print(
				"- finish piece " +
				std::to_string(m_ID) +
				" input move");
		});

	//start to input move
	StartBH(BH_Input);
}




void Piece::StartMove(int squareID)
{
	//update actpoint
	m_ActPoint--;
	m_UIShowActpoint->StartNumberChangeAnime(m_ActPoint);

	//update square
	Square* newSqr = Board::Instance()->Squares[squareID];
	Square* oldSqr = m_FootOnSquare;
	m_FootOnSquare = nullptr;
	oldSqr->m_Piece = nullptr;
	m_FootOnSquare = newSqr;
	newSqr->m_Piece = this;

	//start show piece move
	PieceShowMove* move = new PieceShowMove(this);
	move->Reset(newSqr->m_SquareObject->GetTransform()->GetWorldPosition() + OFFSET_PIECE_ON_SQUARE);
	move->SetEndEvent("NotifyCountPlayerFinished", 
		[this]()
		{
			NetworkManager::Instance()->Client_NotifyCountPlayerFinished();
		});
	StartBH(move);
}

void Piece::StartCameraLookAtThis(float duration, std::function<void()> endEvent)
{
	//make camera anime
	{
		//calculate end pos and look at
		D3DXVECTOR3 endPos;
		Camera* cmr = CameraManager::GetMainCamera();
		{
			D3DXVECTOR3 farPos = Board::Instance()->m_CameraOrientation->GetTransform()->GetWorldPosition();
			D3DXVECTOR3 piecePos = m_PieceObject->GetUITransform(PieceObject::UITID::PAWN_CENTER)->GetTransform()->GetWorldPosition();
			D3DXVECTOR3 dir = piecePos - farPos;
			float len = 12.5f;
			D3DXVec3Normalize(&dir, &dir);

			//set end pos
			endPos = piecePos + dir * len;

			//set cmr look at
			cmr->m_CmrLookAt->m_TargetTransform=
				Board::Instance()->m_CameraOrientation->GetTransform();
				//m_PieceObject->GetUITransform(PieceObject::UITID::PAWN_CENTER)->GetTransform();
			cmr->m_IsOrtho = false;
		}

		//create anime
		Animator* animator = new Animator(cmr->GetGameObject());
		animator->AddEndEvent(endEvent);
		AniDesc_Vec3StartEnd desc;
		{
			desc.Duration = duration;
			desc.Start = cmr->GetGameObject()->GetTransform()->GetWorldPosition();
			desc.End = endPos;
		}
		//ComputeSlowLerpVec3 computeFunc;
		ComputeSlowStartSlowEndVec3 computeFunc;
		new Anime_Position(animator, desc, computeFunc);
	}
}

void Piece::StartCaught(int prisonSquareID)
{
	//update square
	Square* newSqr = Board::Instance()->Squares[prisonSquareID];
	//Square* oldSqr = FootOnSquare;//oldSqr has been set by move piece
	m_FootOnSquare = newSqr;
	newSqr->m_Piece = this;

	//start show piece caught
	PieceShowCaught* caught = new PieceShowCaught(this);
	caught->Reset(newSqr->m_SquareObject->
		GetTransform()->GetWorldPosition() + OFFSET_PIECE_ON_SQUARE);
	caught->SetEndEvent("NotifyCountPlayerFinished", 
		[this]()
		{
			NetworkManager::Instance()->Client_NotifyCountPlayerFinished();
		});
	StartBH(caught);
}

void Piece::StartEscape(int escapeSquareID)
{
	//update square
	Square* newSqr = Board::Instance()->Squares[escapeSquareID];
	//Square* oldSqr = FootOnSquare;//oldSqr has been set by piece move
	m_FootOnSquare = newSqr;
	newSqr->m_Piece = this;

	//start show piece escape
	PieceShowEscape* escape = new PieceShowEscape(this);
	escape->Reset(newSqr->m_SquareObject->
		GetTransform()->GetWorldPosition() + OFFSET_PIECE_ON_SQUARE);
	escape->SetEndEvent("NotifyCountPlayerFinished", 
		[this]()
		{
			NetworkManager::Instance()->Client_NotifyCountPlayerFinished();
		});
	StartBH(escape);
}





void Piece::SetUISelect(bool show)
{
	if (show)
	{
		if (!m_SelectedMark)
		{
			UISelect* selectMark = new UISelect();
			selectMark->GetTransform()->SetRotation(PI / 2, 0, 0);
			GameObject* parent = m_PieceObject->
				GetUITransform(PieceObject::UITID::ROOT);
			D3DXVECTOR3 targetPos = parent->GetTransform()->GetWorldPosition();
			selectMark->GetTransform()->SetPosition(targetPos);

			m_SelectedMark = selectMark;
		}
	}
	else
	{
		if (m_SelectedMark)
		{
			m_SelectedMark->SetState(GameObject::DEAD);
			m_SelectedMark = nullptr;
		}
	}
}

void Piece::SetUIFinish(bool show)
{
	if (show)
	{
		if (!m_FinishMark)
		{
			UIFinishMark* finishMark = new UIFinishMark();
			finishMark->FollowWorldObject->SetTargetTransform(
				m_PieceObject->GetUITransform(PieceObject::UITID::PAWN_CENTER)->GetTransform());

			m_FinishMark = finishMark;
		}
	}
	else
	{
		if (m_FinishMark)
		{
			m_FinishMark->SetState(GameObject::DEAD);
			m_FinishMark = nullptr;
		}
	}
}

void Piece::SetUIThink(bool show)
{
	if (show)
	{
		if (!m_ThinkMark)
		{
			UIThinkMark* thinkMark = new UIThinkMark();
			thinkMark->FollowWorldObject->SetTargetTransform(
				m_PieceObject->GetUITransform(PieceObject::UITID::PAWN_CENTER)->GetTransform());

			m_ThinkMark = thinkMark;
		}
	}
	else
	{
		if (m_ThinkMark)
		{
			m_ThinkMark->SetState(GameObject::DEAD);
			m_ThinkMark = nullptr;
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
		if (!m_UIShowHands.empty())
		{
			for (auto obj : m_UIShowHands)
			{
				obj->SetState(GameObject::DEAD);
			}
			m_UIShowHands.clear();
		}
	}
}

void Piece::SetUIActpoint(bool show)
{
	if (show)
	{
		if (!m_UIShowActpoint)
		{
			UIActPoint* uiActpoint = new UIActPoint();
			uiActpoint->m_FollowWorldObject->SetTargetTransform(
				m_PieceObject->GetUITransform(PieceObject::UITID::PAWN_UP_RIGHT)->GetTransform());

			m_UIShowActpoint = uiActpoint;
		}
	}
	else
	{
		if (m_UIShowActpoint)
		{
			m_UIShowActpoint->SetState(GameObject::DEAD);
			m_UIShowActpoint = nullptr;
		}
	}
}

std::string CampTypeName(CampType type)
{
	switch (type)
	{
	case CampType::BAD:
		return std::string("bad");

	case CampType::GOOD:
		return std::string("good");
	}
}
