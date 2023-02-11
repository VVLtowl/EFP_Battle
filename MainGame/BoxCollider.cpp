#include "main.h"
#include "BoxCollider.h"

BoxCollider::BoxCollider(GameObject* owner,
	BoxColliderDescription boxColDesc,
	int updateOrder):
	Collider(owner, updateOrder),
	AABBData(boxColDesc.AABBData)
{
	m_ColliderType = COL_AABB;

#ifdef HINT_BOX_COLLIDER 
	m_BoxVertexLocalPos[0] = { AABBData.Min[0],AABBData.Max[1],AABBData.Max[2] };
	m_BoxVertexLocalPos[1] = { AABBData.Max[0],AABBData.Max[1],AABBData.Max[2] };
	m_BoxVertexLocalPos[2] = { AABBData.Min[0],AABBData.Max[1],AABBData.Min[2] };
	m_BoxVertexLocalPos[3] = { AABBData.Max[0],AABBData.Max[1],AABBData.Min[2] };
	m_BoxVertexLocalPos[4] = { AABBData.Min[0],AABBData.Min[1],AABBData.Max[2] };
	m_BoxVertexLocalPos[5] = { AABBData.Max[0],AABBData.Min[1],AABBData.Max[2] };
	m_BoxVertexLocalPos[6] = { AABBData.Min[0],AABBData.Min[1],AABBData.Min[2] };
	m_BoxVertexLocalPos[7] = { AABBData.Max[0],AABBData.Min[1],AABBData.Min[2] };

	for (int i = 0; i < 12; i++)
	{
		DrawLineDescription desc;
		desc.Size = 0.05f;
		desc.Color = { 10,1,1,1 };
		m_DrawLine[i] = new DrawLine(m_Owner, desc);
	}
#endif
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::Update()
{
	//prepare draw box by line
#ifdef HINT_BOX_COLLIDER 
	{
		m_DrawLine[0]->LineData.StartBH = m_BoxVertexLocalPos[0]; 
		m_DrawLine[0]->LineData.End = m_BoxVertexLocalPos[1];
		m_DrawLine[1]->LineData.StartBH = m_BoxVertexLocalPos[1]; 
		m_DrawLine[1]->LineData.End = m_BoxVertexLocalPos[3];
		m_DrawLine[2]->LineData.StartBH = m_BoxVertexLocalPos[3];
		m_DrawLine[2]->LineData.End = m_BoxVertexLocalPos[2];
		m_DrawLine[3]->LineData.StartBH = m_BoxVertexLocalPos[2];
		m_DrawLine[3]->LineData.End = m_BoxVertexLocalPos[0];


		m_DrawLine[4]->LineData.StartBH = m_BoxVertexLocalPos[0];
		m_DrawLine[4]->LineData.End = m_BoxVertexLocalPos[4];
		m_DrawLine[5]->LineData.StartBH = m_BoxVertexLocalPos[1];
		m_DrawLine[5]->LineData.End = m_BoxVertexLocalPos[5];
		m_DrawLine[6]->LineData.StartBH = m_BoxVertexLocalPos[3];
		m_DrawLine[6]->LineData.End = m_BoxVertexLocalPos[7];
		m_DrawLine[7]->LineData.StartBH = m_BoxVertexLocalPos[2];
		m_DrawLine[7]->LineData.End = m_BoxVertexLocalPos[6];


		m_DrawLine[8]->LineData.StartBH = m_BoxVertexLocalPos[4];
		m_DrawLine[8]->LineData.End = m_BoxVertexLocalPos[5];
		m_DrawLine[9]->LineData.StartBH = m_BoxVertexLocalPos[5];
		m_DrawLine[9]->LineData.End = m_BoxVertexLocalPos[7];
		m_DrawLine[10]->LineData.StartBH = m_BoxVertexLocalPos[7];
		m_DrawLine[10]->LineData.End = m_BoxVertexLocalPos[6];
		m_DrawLine[11]->LineData.StartBH = m_BoxVertexLocalPos[6];
		m_DrawLine[11]->LineData.End = m_BoxVertexLocalPos[4];

	}
#endif

}
