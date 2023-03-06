#include "main.h"
#include "Button.h"
#include "Character.h"
#include "Piece.h"
#include "MyButtonGroup.h"


#include "Piece.h"
#include "MyNetManager.h"

#include "ButtonObject.h"

#pragma region ========== button hand group ==========
ButtonHandGroup::ButtonHandGroup(GameObject* owner) :
	ButtonGroup(owner)
{
}

void ButtonHandGroup::OtherUpdate()
{
	//mouse hint
	{
		/*	D3DXVECTOR3 endPos, startPos;
			Camera* cmr = CameraManager::GetMainCamera();

			//get mouse pos, compute pos in camera's world
			{
				POINT pt = Input::Mouse.GetMouseWinPos();
				if (cmr->IsOrtho == true)
				{
					//compute pos in camera's world
					{
						startPos = {
							(float)(pt.x - SCREEN_WIDTH / 2) / SCREEN_WIDTH * cmr->Width,
							(float)(SCREEN_HEIGHT / 2 - pt.y) / SCREEN_HEIGHT * cmr->Height,
							0 };
						endPos = startPos;
						endPos.z += 100;
					}
				}
				else
				{
					//compute pos in camera's world
					{
						float aspect = cmr->Width / cmr->Height;
						float tanHalfFovy = tanf(cmr->Fovy / 2 * PI / 180);

						float nearHeight = tanHalfFovy * cmr->ZNear * 2;
						float nearWidth = nearHeight * aspect;

						//float tanHalfFovx = nearWidth / 2 / cmr->ZNear;

						float farHeight = tanHalfFovy * cmr->ZFar * 2;
						float farWidth = farHeight * aspect;

						startPos = V3_ZERO;
						endPos = {
							(float)(pt.x - SCREEN_WIDTH / 2) / SCREEN_WIDTH * farWidth,
							(float)(SCREEN_HEIGHT / 2 - pt.y) / SCREEN_HEIGHT * farHeight,
							cmr->ZFar };
					}
				}
			}

			//compute pos to world
			{
				const D3DXMATRIX& cmrWorld = cmr->GetGameObject()->GetTransform()->GetWorldMatrix();
				D3DXVec3TransformCoord(&startPos, &startPos, &cmrWorld);
				D3DXVec3TransformCoord(&endPos, &endPos, &cmrWorld);
			}

			//hint mouse point ray
	#ifdef HINT_MOUSE_RAY
			{
				m_MouseHintLine->StartPos = startPos;
				m_MouseHintLine->EndPos = endPos;
			}
	#endif*/

	//check button event by ray and boxCollider
		/*{
			Collision::Line line = MousePointRay::Instance()->GetLine();// line(startPos, endPos);
			float outDist;
			D3DXVECTOR3 outPoint;
			D3DXVECTOR3 outNormal;

			ButtonHand* btn;
			auto iter = m_ButtonObjects.begin();
			auto end = m_ButtonObjects.end();
			for (; iter != end; iter++)
			{
				btn = ((ButtonHand*)(*iter));
				const Collision::AABB& aabb = btn->GetAABB();
				const D3DXMATRIX& boxWorld = btn->GetTransform()->GetWorldMatrix();

				if (Collision::Intersect(line, aabb, boxWorld,
					&outDist, &outPoint, &outNormal))
				{
					if (Input::Mouse.IsButtonPress(MOUSE_LEFT))
					{
						btn->GetButton()->ChangeState(Button::CLICKED);
					}
					else
					{
						btn->GetButton()->ChangeState(Button::HIGHLIGHTED);
					}
				}
				else
				{
					btn->GetButton()->ChangeState(Button::IDLE);
				}
			}
		}*/
	}
}

void ButtonHandGroup::InitHandButtonsClickEvent()
{
	ButtonHand* btnHand = nullptr;
	for (auto obj : m_ButtonObjects)
	{
		btnHand = (ButtonHand*)obj;
		btnHand->GetButton()->AddClickEvent([this, btnHand]()
			{
				HandType handType = btnHand->Type;
				Piece* piece = btnHand->TargetPiece;
				//TargetPiece->Hands.emplace_back(handType);
				//start wait judgement check
				GetNetSendFunc().Piece_RequestSetHand(
					piece->m_ID,
					handType);

				//while one click, finish this ui control
				ChangeState(State::FINISH);
			});
	}
}
#pragma endregion

#pragma region ========== button select act group ==========
ButtonSelectActGroup::ButtonSelectActGroup(GameObject* owner) :
	ButtonGroup(owner)
{
}

void ButtonSelectActGroup::OtherUpdate()
{
}
#pragma endregion
