#include "main.h"
#include "Button.h"
#include "TransformAnime.h"
#include "LookAtCamera.h"
#include "SceneManager.h"
#include "ActSelection.h"

ActSelection::ActSelection(const D3DXVECTOR3& pos, const D3DXVECTOR3& scl):
	GameObject(),
	OriginalPosition(pos),
	OriginalScale(scl)
{	
	//init transform
	{
		m_Transform3D->SetPosition(pos);
		m_Transform3D->SetScale(scl);
	}

	//create polygon2D
	{
		Polygon2DDescription polyDesc;
		polyDesc.DrawObjDesc.DepthEnable = true;
		polyDesc.DrawObjDesc.Layer = LAYER_WORLD_NORMAL;//LAYER_WORLD_SHADOW_ALPHA;
		polyDesc.DrawObjDesc.Shader = SHADER_UNLIT;//SHADER_SHADOW;
		polyDesc.TextureID = TEXID_HINT;
		polyDesc.Size = ACTSELECTION_IDLE_SIZE;
		m_Polygon2D = new Polygon2D(this, polyDesc);
	}

	//create boxCollider
	{
		BoxColliderDescription boxDesc;
		boxDesc.AABBData.Min = {
			-ACTSELECTION_IDLE_SIZE.x / 2.0f,
			-ACTSELECTION_IDLE_SIZE.y / 2.0f,
			-0.1f
		};
		boxDesc.AABBData.Max = {
			ACTSELECTION_IDLE_SIZE.x / 2.0f,
			ACTSELECTION_IDLE_SIZE.y / 2.0f,
			0.1f
		};
		m_BoxCollider = new BoxCollider(this, boxDesc);
	}

	//create button
	{
		m_Button = new Button(this);
		m_Button->AddEnterEvent([this]()
			{
				m_Polygon2D->Color = HighlightColor;

				TransformAnimeDescripition transDesc;
				transDesc.LoopCount = 1;
				transDesc.Duration = 15;
				transDesc.StartPosition = m_Transform3D->GetPosition();
				transDesc.EndScale = m_Transform3D->GetScale();
				transDesc.EndPosition = m_Transform3D->GetPosition(); //OriginalPosition + ACTSELECTION_HIGHLIGHT_MOVE;
				transDesc.EndScale = OriginalScale * ACTSELECTION_HIGHLIGHT_SCALE;
				new TransformAnime(this, transDesc);

				//DebugInfo::Print("highlight button");
			});
		m_Button->AddExitEvent([this]()
			{
				m_Polygon2D->Color = OriginalColor;

				TransformAnimeDescripition transDesc;
				transDesc.LoopCount = 1;
				transDesc.Duration = 15;
				transDesc.StartPosition = m_Transform3D->GetPosition();
				transDesc.EndScale = m_Transform3D->GetScale();
				transDesc.EndPosition = m_Transform3D->GetPosition(); //OriginalPosition;
				transDesc.EndScale = OriginalScale;
				new TransformAnime(this, transDesc);
				
				//DebugInfo::Print("reset button");
			});
	}



	//create lookAtCamera
	{
		Camera* cmr = CameraManager::GetMainCamera();
		m_LookAtCamera = new LookAtCamera(this, cmr);
	}

	//create draw line
	if(0)
	{
		Camera* cmr = CameraManager::GetMainCamera();
		DrawLineDescription drawlDesc;
		drawlDesc.Color = { 3,0,0,1 };
		drawlDesc.Size = 0.05f;
		drawlDesc.StartPos = cmr->GetGameObject()->GetTransform()->GetPosition();
		drawlDesc.LocalPos = false;
		m_DrawLine = new DrawLine(this, drawlDesc);
	}

	//create hint
	m_Hint = GameObjectManager::Create<Emitter>();// GameObjectManager::Create<ParticleSystem>();// 
}

ActSelection::~ActSelection()
{
	//m_Hint->SetState(DEAD);

}

void ActSelection::UpdateGameObject()
{
	//mouse hint
	{
		Collision::Line line = MousePointRay::Instance()->GetLine();
		Collision::AABB aabb =m_BoxCollider->AABBData;
		D3DXMATRIX boxWorld = m_Transform3D->GetWorldMatrix();


		//check collision
		float outDist;
		D3DXVECTOR3 outPoint;
		D3DXVECTOR3 outNormal;
		if (Collision::Intersect(line, aabb, boxWorld,
			&outDist, &outPoint, &outNormal))
		{
			if (Input::Mouse.IsButtonDown(MOUSE_LEFT))
			{
				m_Button->ChangeState(Button::State::CLICKED);
				return;
			}
			else
			{
				m_Button->ChangeState(Button::State::HIGHLIGHTED);
				//set hint
				{
					m_Hint->GetTransform()->SetPosition(
						line.Start + 0.5f * (line.End - line.Start));
					m_Hint->GetTransform()->SetPosition(outPoint);
				}
			}		
		}
		else
		{
			m_Button->ChangeState(Button::State::IDLE);
			//set hint
			{
				//m_Hint->GetTransform()->SetPosition(line.Start);
				m_Hint->GetTransform()->SetPosition(0,-1000,0);
			}
		}

	}
}
