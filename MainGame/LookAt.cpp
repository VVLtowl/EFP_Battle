#include "main.h"
#include "LookAt.h"

LookAt::LookAt(GameObject* owner, Transform3D* target, int order):
	Component(owner,order),
	m_TargetTransform(target)
{
}

LookAt::~LookAt()
{
}

void LookAt::Update()
{
	if (m_TargetTransform == nullptr)
	{
		return;
	}


	m_Owner->GetTransform()->DisableUpdateThisFrame();

	D3DXMATRIX rot;
	D3DXMatrixIdentity(&rot);
	D3DXVECTOR3 eye = m_Owner->GetTransform()->GetWorldPosition();
	D3DXVECTOR3 lookAt = m_TargetTransform->GetWorldPosition();
	D3DXMatrixLookAtLH(&rot, &eye, &lookAt, &V3_AXIS_Y);

	D3DXMatrixInverse(&rot, nullptr, &rot);
	rot._41 = 0;
	rot._42 = 0;
	rot._43 = 0;

	D3DXMATRIX translate,scale;
	D3DXVECTOR3 pos = m_Owner->GetTransform()->GetWorldPosition();
	D3DXVECTOR3 scl = m_Owner->GetTransform()->GetWorldScale();
	D3DXMatrixTranslation(&translate, pos.x, pos.y, pos.z);
	D3DXMatrixScaling(&scale, scl.x, scl.y, scl.z);
	D3DXMATRIX world = scale * rot * translate;

	m_Owner->GetTransform()->SetWorldMatrix(world);
}

