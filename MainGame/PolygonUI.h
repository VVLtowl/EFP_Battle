#pragma once

#include "DrawObject.h"
#include <string>

/*********************************************************
* @brief	UI描画対象を作るための説明
********************************************************/
struct PolygonUIDescription
{
	DrawObjectDescription DrawObjDesc;
	D3DXVECTOR2 Size = D3DXVECTOR2(1280, 720) * 0.25f;
	D3DXVECTOR4 Color = {1,1,1,1};
	//std::string TextureName = "asset\\texture\\number.dds";
	short TextureID = 0;

	PolygonUIDescription()
	{
		DrawObjDesc.Shader = SHADER_UNLIT;
		DrawObjDesc.Layer = LAYER_SCREEN_UI;
	}
};

/*********************************************************
* @brief	UIのポリゴン
********************************************************/
class PolygonUI :
	public DrawObject
{
private:
	/*********************************************************
	* @brief	動的確保する要素
	********************************************************/
	ID3D11Buffer* m_VertexBuffer;

public:	
	/*********************************************************
	* @brief	好み設定のできる要素
	********************************************************/
	D3DXVECTOR2 Size;
	D3DXVECTOR4 Color;
	ID3D11ShaderResourceView* Texture;

public:
	void Draw()override;
	PolygonUI(
		class GameObject* owner,
		const PolygonUIDescription& polygonUIDesc,
		int order = COMP_POLYGONUI);
	~PolygonUI();
};

