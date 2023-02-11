#include "main.h"
#include "Sphere.h"

Sphere::Sphere()
{
	//set name
	//SetName("Sphere");

	//set scale
	D3DXVECTOR3 scale = V3_ONE * 40;
	m_Transform3D->SetScale(scale);

	//creaet polygon3d
	Polygon3DDescription desc;
	desc.DrawObjDesc.Layer = LAYER_WORLD_NORMAL;
	desc.DrawObjDesc.Shader = SHADER_UNLIT;
	desc.ModelID = MDLID_SPHERE;
	m_Polygon3D = new Polygon3D(this, desc);

}

void Sphere::UpdateGameObject()
{
}
