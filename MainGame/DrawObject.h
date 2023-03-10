#pragma once
#include "Component.h"
#include <string>

/*********************************************************
* @brief	描画レイヤー
********************************************************/
enum LayerType
{
	LAYER_WORLD_SHADOW_NORMAL,
	LAYER_WORLD_SHADOW_ALPHA,
	LAYER_WORLD_PLAYER_ACTMARK,
	LAYER_WORLD_SHADOW_PLAYER,
	LAYER_WORLD_NORMAL,
	LAYER_WORLD_ALPHA,

	LAYER_WORLD_UI,
	LAYER_WORLD_PARTICLE,
	LAYER_WORLD_LINE_TOOL,

	LAYER_SCREEN_UI,
	LAYER_WORLD_BUTTON,
	LAYER_SCREEN_START_UI,

	LAYER_MAX,
};

/*********************************************************
* @brief	描画対象を作るための説明
********************************************************/
struct DrawObjectDescription
{
	LayerType Layer = LAYER_WORLD_NORMAL;
	ShaderType Shader = SHADER_BLINN;
	bool DepthEnable = true;
	D3DXVECTOR4 Texcoord = { 0,0,1,1 };
};

/*********************************************************
* @brief	画面に描画する対象
********************************************************/
class DrawObject:
	public Component
{
public:
	virtual void Draw() = 0;
	DrawObject(
		class GameObject* owner, 
		const DrawObjectDescription& drawObjDesc,
		int order = COMP_COUNT);
	virtual ~DrawObject();

protected:
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_VertexLayout;
	bool m_DepthEnable;

public:
	LayerType Layer;

	//2D Texcoord
public:
	D3DXVECTOR4 Texcoord;
};
