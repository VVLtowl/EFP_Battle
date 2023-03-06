#include "main.h"
#include "SquareObject.h"

SquareObject::SquareObject()
{
	//transform init
	{
		//m_Transform3D->SetScale(1, 1, 1);
	}

	//create polygon3d
	Polygon3DDescription desc;
	desc.DrawObjDesc.Layer = LAYER_WORLD_SHADOW_NORMAL;
	//desc.DrawObjDesc.PixelShaderName = "DepthShadowMappingPS.cso";
	//desc.DrawObjDesc.VertexShaderName = "DepthShadowMappingVS.cso";
	desc.DrawObjDesc.Shader = SHADER_SHADOW;
	desc.ModelID = MDLID_SQUARE_NORMAL;
	m_Polygon3D = new Polygon3D(this, desc);
}


void SquareObject::UpdateGameObject()
{
}

