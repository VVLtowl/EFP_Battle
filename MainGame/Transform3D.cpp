#include "main.h"
#include "Transform3D.h"
#include "transformManager.h"


Transform3D::Transform3D(GameObject* owner,int updateOrder) :
	Transform(owner, updateOrder),
	m_NeedUpdate(true),
	m_Position(V3_ZERO),
	m_Rotation(QUAT_IDENTITY),
	m_Scale(V3_ONE),
	m_Parent(nullptr),
	m_WorldMatrix(MTX_NOTRANSFORM)
{
}

Transform3D::~Transform3D()
{
}

void Transform3D::Update()
{
	if (m_NeedUpdate == false)
	{
		m_NeedUpdate = true;
	}
	else
	{
		UpdateTransform();
	}
}

void Transform3D::UpdateTransform()
{
	//ワールドマトリクス設定
	D3DXVECTOR3 pos = m_Position;
	D3DXQUATERNION rot = m_Rotation;
	D3DXVECTOR3 scl = m_Scale;
	D3DXMATRIX world, scale, rotate, translate;
	D3DXMatrixScaling(&scale, scl.x, scl.y, scl.z);
	D3DXMatrixRotationQuaternion(&rotate, &rot);
	D3DXMatrixTranslation(&translate, pos.x, pos.y, pos.z);

	if (m_Parent == nullptr)
	{
		world = scale * rotate * translate;
	}
	else
	{
		D3DXMATRIX parentWorld = m_Parent->GetWorldMatrix();
		world = scale * rotate * translate * parentWorld;
	}

	m_WorldMatrix = world;
}

D3DXVECTOR3 Transform3D::GetWorldScale()const
{
	D3DXVECTOR3 scale = m_Scale;

	if (m_Parent != nullptr)
	{
		D3DXVECTOR3 parentScale = m_Parent->GetWorldScale();
		scale.x *= parentScale.x; 
		scale.y *= parentScale.y;
		scale.z *= parentScale.z;
	}

	return scale;
}

D3DXVECTOR3 Transform3D::GetWorldPosition()const
{
	D3DXMATRIX translate;
	D3DXVECTOR3 pos = m_Position;
	D3DXMatrixTranslation(&translate, pos.x, pos.y, pos.z);
	if (m_Parent != nullptr)
	{
		translate *= m_Parent->GetWorldMatrix();
	}


	D3DXVECTOR3 worldPos;
	worldPos.x = translate._41;
	worldPos.y = translate._42;
	worldPos.z = translate._43;

	return worldPos;
}

D3DXVECTOR3 Transform3D::GetForward()const
{
	D3DXMATRIX rot;
	if (1)
	{
		rot = m_WorldMatrix;
	}
	else
	{
		D3DXMatrixRotationQuaternion(&rot, &m_Rotation);
	}
	if (m_Parent != nullptr)
	{
		rot *= m_Parent->GetWorldMatrix();
	}
	//D3DXMatrixRotationYawPitchRoll(
	//	&rot,
	//	m_Rotation.y,
	//	m_Rotation.x,
	//	m_Rotation.z);

	D3DXVECTOR3 dir;
	dir.x = rot._31;
	dir.y = rot._32;
	dir.z = rot._33;

	//dir.x = rot._13;
	//dir.y = rot._23;
	//dir.z = rot._33;
	
	D3DXVec3Normalize(&dir, &dir);

	return dir;
}

D3DXVECTOR3 Transform3D::GetUp()const
{
	D3DXMATRIX rot = m_WorldMatrix;
	if (m_Parent != nullptr)
	{
		rot *= m_Parent->GetWorldMatrix();
	}
	//D3DXMatrixRotationYawPitchRoll(
	//	&rot,
	//	m_Rotation.y,
	//	m_Rotation.x,
	//	m_Rotation.z);

	D3DXVECTOR3 dir;
	dir.x = rot._21;
	dir.y = rot._22;
	dir.z = rot._23;

	D3DXVec3Normalize(&dir, &dir);

	return dir;
}

D3DXVECTOR3 Transform3D::GetRight()const
{
	D3DXMATRIX rot = m_WorldMatrix;
	if (m_Parent != nullptr)
	{
		rot *= m_Parent->GetWorldMatrix();
	}
	//D3DXMatrixRotationYawPitchRoll(
	//	&rot,
	//	m_Rotation.y,
	//	m_Rotation.x,
	//	m_Rotation.z);

	D3DXVECTOR3 dir;
	dir.x = rot._11;
	dir.y = rot._12;
	dir.z = rot._13;

	D3DXVec3Normalize(&dir, &dir);

	return dir;
}

void Transform3D::SetForwardTo(const D3DXVECTOR3& targetPos)
{
	// Figure out difference between original (unit x) and new
	D3DXVECTOR3 dir = targetPos - m_Position;
	D3DXVECTOR3 unitDir = dir;
	D3DXVec3Normalize(&unitDir, &dir);
	float dot = D3DXVec3Dot(&V3_AXIS_Z, &unitDir);//dot (-1,1)
	float radian = acos(dot);//radian (0,PI)
	// Facing down X
	if (dot > 0.9999f)
	{
		SetRotation(0, 0, 0);
	}
	// Facing down -X
	else if (dot < -0.9999f)
	{
		SetRotation(0, PI, 0);
	}
	else
	{
		// Rotate about axis from cross product
		D3DXVECTOR3 axis;
		D3DXVec3Cross(&axis, &V3_AXIS_Z, &unitDir);
		D3DXVec3Normalize(&axis, &axis);
		D3DXQUATERNION quat = QUAT_IDENTITY;
		D3DXQuaternionRotationAxis(&quat, &axis, radian);
		SetRotation(quat);
	}
}

void Transform3D::LookAtView(const D3DXMATRIX& view)
{
	D3DXMATRIX viewTranspose;
	D3DXMatrixIdentity(&viewTranspose);
	viewTranspose._11 = view._11;
	viewTranspose._12 = view._21;
	viewTranspose._13 = view._31;
	viewTranspose._21 = view._12;
	viewTranspose._22 = view._22;
	viewTranspose._23 = view._32;
	viewTranspose._31 = view._13;
	viewTranspose._32 = view._23;
	viewTranspose._33 = view._33;

	Update();

	D3DXMATRIX noRot;
	D3DXMatrixIdentity(&noRot);
	noRot._41 = m_WorldMatrix._41;
	noRot._42 = m_WorldMatrix._42;
	noRot._43 = m_WorldMatrix._43;
	D3DXVECTOR3 worldScale = GetWorldScale();
	noRot._11 = worldScale.x;
	noRot._22 = worldScale.y;
	noRot._33 = worldScale.z;

	m_WorldMatrix = viewTranspose * noRot;
}

void Transform3D::LookAt(const D3DXVECTOR3& targetPos)
{
	D3DXMATRIX rot;
	D3DXMatrixIdentity(&rot);
	D3DXMatrixLookAtLH(
		&rot,
		&m_Position,
		&targetPos,
		&V3_AXIS_Y
	);
	D3DXMatrixInverse(&rot, nullptr, &rot);
	rot._41 = 0;
	rot._42 = 0;
	rot._43 = 0;

	Update();

	D3DXMATRIX noRot;
	D3DXMatrixIdentity(&noRot);
	noRot._41 = m_WorldMatrix._41;
	noRot._42 = m_WorldMatrix._42;
	noRot._43 = m_WorldMatrix._43;
	D3DXVECTOR3 worldScale = GetWorldScale();
	noRot._11 = worldScale.x;
	noRot._22 = worldScale.y;
	noRot._33 = worldScale.z;

	m_WorldMatrix = rot * noRot;
}