#include "main.h"
#include "drawManager.h"
#include "DrawObject.h"

DrawObject::DrawObject(GameObject* owner, const DrawObjectDescription& drawObjDesc, int order):
	Component(owner,order)
{

#if 0
	//頂点シェーダー生成
	Renderer::CreateVertexShader(
		&m_VertexShader,
		&m_VertexLayout,
		drawObjDesc.VertexShaderName.c_str()
	);

	//ピックセルシェーダー生成
	Renderer::CreatePixelShader(
		&m_PixelShader,
		drawObjDesc.PixelShaderName.c_str()
	);
#endif
	int shaderType = drawObjDesc.Shader;
	m_VertexShader = DrawManager::VertexShader[shaderType];
	m_VertexLayout = DrawManager::VertexLayout[shaderType];
	m_PixelShader = DrawManager::PixelShader[shaderType];

	m_DepthEnable = drawObjDesc.DepthEnable;

	//set draw layer
	Layer = drawObjDesc.Layer;

	//set 2d texcoord
	Texcoord = drawObjDesc.Texcoord;

	//register to draw manager
	DrawManager::Register(this);
}

DrawObject::~DrawObject()
{
	m_PixelShader = nullptr;
	m_VertexShader = nullptr;
	m_VertexLayout = nullptr;

	//quit from draw manager
	DrawManager::Quit(this);
}
