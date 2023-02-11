#include "main.h"
#include "MoveControl.h"
#include "LineCollisionTester.h"

LineCollisionTester::LineCollisionTester()
{
	//create end object
	m_CollisionHint = GameObjectManager::Create<Emitter>();
	m_StartHint = GameObjectManager::Create<Torus>();
	m_StartHint->GetTransform()->SetScale(0.2f, 0.1f, 0.1f);
	m_EndHint = GameObjectManager::Create<Torus>();
	m_EndHint->GetTransform()->SetScale(0.1f, 0.1f, 0.1f);

	//move control
	new MoveControl(m_StartHint);
	new MoveControl(
		m_EndHint,
		[]() {return Input::Keyboard.IsKeyPress(KEY_J); },
		[]() {return Input::Keyboard.IsKeyPress(KEY_L); },
		[]() {return Input::Keyboard.IsKeyPress(KEY_U); },
		[]() {return Input::Keyboard.IsKeyPress(KEY_O); },
		[]() {return Input::Keyboard.IsKeyPress(KEY_I); },
		[]() {return Input::Keyboard.IsKeyPress(KEY_K); }
	);

	//create drawline
	DrawLineDescription desc;
	desc.StartTransform = m_StartHint->GetTransform();
	desc.EndTransform = m_CollisionHint->GetTransform();
	desc.Color = { 5,2,2,1 };
	desc.Size = 0.01f;
	desc.DrawObjDesc.DepthEnable = true;
	m_DrawLine = new DrawLine(this,desc);
}

LineCollisionTester::~LineCollisionTester()
{
	m_StartHint->SetState(DEAD);
	m_EndHint->SetState(DEAD);
	m_CollisionHint->SetState(DEAD);
}

void LineCollisionTester::UpdateGameObject()
{
	//test
	//collision line with aabb
	D3DXVECTOR3 colPos = m_EndHint->GetTransform()->GetPosition();
	m_DrawLine->LineData.End = colPos;
	for (auto col : ColliderManager::Colliders)
	{
		if (typeid(*col) == typeid(BoxCollider))
		{
			BoxCollider* box = (BoxCollider*)col;

			Collision::AABB aabb = box->AABBData;
			Collision::Sphere sphere(box->GetGameObject()->GetTransform()->GetPosition(), 0.5f);
			Collision::Line line = m_DrawLine->LineData;
			float outDist;
			D3DXVECTOR3 outPoint;
			D3DXVECTOR3 outNormal;
			D3DXMATRIX boxWorld = box->GetGameObject()->GetTransform()->GetWorldMatrix();
			if (Collision::Intersect(line, aabb, boxWorld,
				&outDist, &outPoint, &outNormal))
			//if(Collision::Intersect(line, sphere,&outDist,&outPoint))
			{
				DebugInfo::Print("col test");

				colPos = outPoint;
				break;
			}
		}
	}
	m_CollisionHint->GetTransform()->SetPosition(colPos);
}
