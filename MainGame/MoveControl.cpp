#include "main.h"
#include "MoveControl.h"

MoveControl::MoveControl(GameObject* owner,
	std::function<bool()> moveLeft,
	std::function<bool()> moveRight,
	std::function<bool()> moveUp,
	std::function<bool()> moveDown,
	std::function<bool()> moveForward,
	std::function<bool()> moveBack,
	int updateOrder):
	Component(owner,updateOrder),
	m_MoveLeft(moveLeft),
	m_MoveRight(moveRight),
	m_MoveUp(moveUp),
	m_MoveDown(moveDown),
	m_MoveForward(moveForward),
	m_MoveBack(moveBack),
	TargetTransform(nullptr),
	m_Speed(0.1f)
{
}

MoveControl::~MoveControl()
{
}

void MoveControl::Update()
{
	//test
	//not for world position
	Transform3D* trans = TargetTransform == nullptr ?
		m_Owner->GetTransform() : TargetTransform;
	D3DXVECTOR3 pos = trans->GetPosition();
	D3DXVECTOR3 dir = V3_ZERO;
	if (m_MoveLeft())
	{
		dir.x = -1;
	}
	if (m_MoveRight())
	{
		dir.x = 1;
	}
	if (m_MoveUp())
	{
		dir.y = 1;
	}
	if (m_MoveDown())
	{
		dir.y = -1;
	}
	if (m_MoveForward())
	{
		dir.z = 1;
	}
	if (m_MoveBack())
	{
		dir.z = -1;
	}
	D3DXVec3Normalize(&dir, &dir);
	pos += dir * m_Speed;
	trans->SetPosition(pos);
}
