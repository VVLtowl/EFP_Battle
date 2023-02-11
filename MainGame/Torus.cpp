#include "main.h"
#include "Torus.h"


Torus::Torus()
{	
	//set name
	SetName("Torus");

	//component
	{
		//•`‰æ‘ÎÛ
		Polygon3DDescription desc;
		desc.DrawObjDesc.Layer = LAYER_WORLD_SHADOW_NORMAL;
		//desc.DrawObjDesc.PixelShaderName = "DepthShadowMappingPS.cso";
		//desc.DrawObjDesc.VertexShaderName = "DepthShadowMappingVS.cso";
		desc.DrawObjDesc.Shader = SHADER_SHADOW;
		desc.ModelName = "asset\\model\\torus.obj";
		m_Polygon3D = new Polygon3D(this, desc);

	}
}

void Torus::UpdateGameObject()
{
	//test
	//m_EulerAngle.x += 0.01f;
	//m_EulerAngle.y += 0.005f;
	//GetTransform()->SetRotation(m_EulerAngle.x, m_EulerAngle.y, m_EulerAngle.z);
}

