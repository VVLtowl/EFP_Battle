#include "main.h"
#include "TransformObject.h"

void TransformObject::UpdateGameObject()
{
	//set name
	//SetName("TransformObject");
}

void TransformObject::InitLocalPosition(const D3DXVECTOR3& localPos)
{
	m_Transform3D->SetPosition(localPos);
}
