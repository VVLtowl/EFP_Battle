#include "main.h"
#include "Plane.h"

Plane::Plane()
{
	//set name
	//SetName("Plane");

	//create polygon2D
	Polygon2DDescription desc;
	desc.DrawObjDesc.Layer = LAYER_WORLD_SHADOW_NORMAL;
	//desc.DrawObjDesc.PixelShaderName = "DepthShadowMappingPS.cso";
	//desc.DrawObjDesc.VertexShaderName = "DepthShadowMappingVS.cso";
	desc.DrawObjDesc.Shader = SHADER_SHADOW;//SHADER_UNLIT;
	desc.TextureID = TEXID_FIELD004;
	desc.Size = { 4,4 };
	Image = new Polygon2D(this, desc);
}

void Plane::UpdateGameObject()
{
}
