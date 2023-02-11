#include "main.h"
#include "BoxCollider.h"
#include "Cube.h"

Cube::Cube()
{
	//set name
	//SetName("Cube");

	//set scale
	D3DXVECTOR3 scale = V3_ONE;
	m_Transform3D->SetScale(scale);

	//creaet polygon3d
	Polygon3DDescription desc;
	desc.DrawObjDesc.Layer = LAYER_WORLD_SHADOW_NORMAL;
	desc.DrawObjDesc.Shader = SHADER_SHADOW; //SHADER_UNLIT;
	desc.ModelID = MDLID_CUBE;
	m_Polygon3D = new Polygon3D(this, desc);

	//create box collider
	BoxColliderDescription boxDesc;
	boxDesc.AABBData.Max = 0.5f * scale;
	boxDesc.AABBData.Min = -0.5f * scale;
	m_BoxCollider = new BoxCollider(this, boxDesc);
}

void Cube::UpdateGameObject()
{
}
