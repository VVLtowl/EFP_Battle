#pragma once

#include "DrawObject.h"
#include <string>

/*********************************************************
* @brief	UI�`��Ώۂ���邽�߂̐���
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
* @brief	UI�̃|���S��
********************************************************/
class PolygonUI :
	public DrawObject
{
private:
	/*********************************************************
	* @brief	���I�m�ۂ���v�f
	********************************************************/
	ID3D11Buffer* m_VertexBuffer;

public:	
	/*********************************************************
	* @brief	�D�ݐݒ�̂ł���v�f
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

