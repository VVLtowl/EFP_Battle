#include "main.h"
#include "Button.h"
#include "ButtonGroup.h"

#include "ButtonObject.h"



ButtonGroup::ButtonGroup(GameObject* owner):
	UIGroupControl(owner),
	ButtonGroupState(State::WORKING),
	OnFinish([]() {; }),
	OnButtonSelected([]() {; })
{
	//create mouse line hint
#ifdef HINT_MOUSE_RAY
	{
		DrawLineDescription desc;
		desc.LocalPos = false;
		desc.Size = 0.05f;
		desc.Color = { 10,1,1,1 };
		m_MouseHintLine = new DrawLine(m_Owner, desc);
	}
#else
	m_MouseHintLine = nullptr;
#endif
}

ButtonGroup::~ButtonGroup()
{
	for (auto btn : m_ButtonObjects)
	{
		btn->SetState(GameObject::DEAD);
	}
	m_ButtonObjects.clear();
}

void ButtonGroup::Update()
{
	if (ButtonGroupState == State::WORKING)
	{
		//choose button by mouse or keyboard
		//mouse
		{
			//moue line hint
			{
				D3DXVECTOR3 endPos, startPos;
				Camera* cmr = CameraManager::GetMainCamera();

				//get mouse pos, compute pos in camera's world
				{
					POINT pt = Input::Mouse.GetMouseWinPos();
					if (cmr->m_IsOrtho == true)
					{
						//compute pos in camera's world
						{
							startPos = {
								(float)(pt.x - SCREEN_WIDTH / 2) / SCREEN_WIDTH * cmr->m_Width,
								(float)(SCREEN_HEIGHT / 2 - pt.y) / SCREEN_HEIGHT * cmr->m_Height,
								0 };
							endPos = startPos;
							endPos.z += 100;
						}
					}
					else
					{
						//compute pos in camera's world
						{
							float aspect = cmr->m_Width / cmr->m_Height;
							float tanHalfFovy = tanf(cmr->m_Fovy / 2 * PI / 180);

							float nearHeight = tanHalfFovy * cmr->m_ZNear * 2;
							float nearWidth = nearHeight * aspect;

							//float tanHalfFovx = nearWidth / 2 / cmr->ZNear;

							float farHeight = tanHalfFovy * cmr->m_ZFar * 2;
							float farWidth = farHeight * aspect;

							startPos = V3_ZERO;
							endPos = {
								(float)(pt.x - SCREEN_WIDTH / 2) / SCREEN_WIDTH * farWidth,
								(float)(SCREEN_HEIGHT / 2 - pt.y) / SCREEN_HEIGHT * farHeight,
								cmr->m_ZFar };
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
#endif
			}

			//check button event by ray and boxCollider
			{
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
					
					//when button is diable, needn't check collision 
					if (btn->GetButton()->GetButtonState() == Button::State::DISABLE)
					{
						continue;
					}
					
					//check collision
					const Collision::AABB& aabb = btn->GetAABB();
					const D3DXMATRIX& boxWorld = btn->GetTransform()->GetWorldMatrix();
					if (Collision::Intersect(line, aabb, boxWorld,
						&outDist, &outPoint, &outNormal))
					{
						if (Input::Mouse.IsButtonPress(MOUSE_LEFT))
						{
							btn->GetButton()->ChangeState(Button::State::CLICKED);
						}
						else
						{
							btn->GetButton()->ChangeState(Button::State::HIGHLIGHTED);
						}
					}
					else
					{
						btn->GetButton()->ChangeState(Button::State::IDLE);
					}
				}
			}
		}

		//keyboard
		{

		}

		//other update
		OtherUpdate();
	}
}

void ButtonGroup::ChangeState(State state)
{
	if (ButtonGroupState == State::WORKING && state == State::FINISH)
	{
		OnFinish();
		ButtonGroupState = state;
	}
}

void ButtonGroup::AddButtonObject(ButtonObject* obj)
{
	m_ButtonObjects.emplace_back(obj);
}

void ButtonGroup::SetUpButtonsParent()
{
	for (auto btn : m_ButtonObjects)
	{
		btn->SetParent(m_Owner);
	}
}
