#include "main.h"
#include "MousePointRay.h"

MousePointRay::MousePointRay()
{
#ifdef HINT_MOUSE_RAY
	//create hint line
	{
		DrawLineDescription desc;
		desc.LocalPos = false;
		desc.Size = 0.05f;
		desc.Color = { 10,1,1,1 };
		m_HintLine = new DrawLine(this, desc);
	}
#else
	m_HintLine = nullptr;
#endif
}

MousePointRay::~MousePointRay()
{
}

void MousePointRay::UpdateGameObject()
{
	//D3DXVECTOR3 endPos, startPos;
	Camera* cmr = CameraManager::GetMainCamera();
	if (cmr == nullptr)
	{
		m_MouseRay.Start =
			m_MouseRay.End = V3_ZERO;
		return;
	}

	//get mouse pos, compute pos in camera's world
	{
		POINT pt = Input::Mouse.GetMouseWinPos();
		if (cmr->m_IsOrtho == true)
		{
			//compute pos in camera's world
			{
				m_MouseRay.Start = {
					(float)(pt.x - SCREEN_WIDTH / 2) / SCREEN_WIDTH * cmr->m_Width,
					(float)(SCREEN_HEIGHT / 2 - pt.y) / SCREEN_HEIGHT * cmr->m_Height,
					0 };
				m_MouseRay.End = m_MouseRay.Start;
				m_MouseRay.End.z += 100;
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

				m_MouseRay.Start = V3_ZERO;
				m_MouseRay.End = {
					(float)(pt.x - SCREEN_WIDTH / 2) / SCREEN_WIDTH * farWidth,
					(float)(SCREEN_HEIGHT / 2 - pt.y) / SCREEN_HEIGHT * farHeight,
					cmr->m_ZFar };
			}
		}
	}

	//compute pos to world
	{
		const D3DXMATRIX& cmrWorld = cmr->GetGameObject()->GetTransform()->GetWorldMatrix();
		D3DXVec3TransformCoord(&m_MouseRay.Start, &m_MouseRay.Start, &cmrWorld);
		D3DXVec3TransformCoord(&m_MouseRay.End, &m_MouseRay.End, &cmrWorld);
	}

	//hint mouse point ray
#ifdef HINT_MOUSE_RAY
	{
		m_HintLine->StartPos = m_MouseRay.Start;
		m_HintLine->EndPos = m_MouseRay.End;
	}
#endif
}

Collision::Line const MousePointRay::GetLine()
{
	return m_MouseRay;
}
