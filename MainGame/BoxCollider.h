#pragma once
#include "Collider.h"



/*********************************************************
* @brief	�{�b�N�X�R���C�_�[����邽�߂̐���
********************************************************/
struct BoxColliderDescription
{
	Collision::AABB AABBData;

	BoxColliderDescription()
	{
	}
};


class BoxCollider:
	public Collider
{
public:
	BoxCollider(class GameObject* owner,
		BoxColliderDescription boxColDesc = BoxColliderDescription(),
		int updateOrder = COMP_BOXCOLI);
	~BoxCollider();
	void Update()override;

public:
	Collision::AABB AABBData;

private:
	D3DXVECTOR3 m_BoxVertexLocalPos[8];
	class DrawLine* m_DrawLine[12];
};

