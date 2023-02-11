#pragma once
#include "DrawObject.h"
#include <list>
#include <memory>

const float LINE_DEFAULT_SIZE = 0.01f;
const D3DXVECTOR4 LINE_DEFAULT_COLOR = { 1,1,1,1 };

/*********************************************************
* @brief	ê¸ÇÃï`âÊëŒè€ÇçÏÇÈÇΩÇﬂÇÃê‡ñæ
********************************************************/
struct DrawLineDescription
{
	DrawObjectDescription DrawObjDesc;
	float Size = LINE_DEFAULT_SIZE;
	D3DXVECTOR4 Color = LINE_DEFAULT_COLOR;
	Transform3D* StartTransform = nullptr;
	Transform3D* EndTransform = nullptr;
	
	bool LocalPos = true;
	D3DXVECTOR3 StartPos = V3_ZERO;
	D3DXVECTOR3 EndPos = V3_ZERO;

	DrawLineDescription()
	{
		DrawObjDesc.Layer = LAYER_WORLD_LINE_TOOL;
		DrawObjDesc.DepthEnable = false;
		DrawObjDesc.Shader = SHADER_UNLIT;
	}
};


/*********************************************************
* @brief	éläpå`ÇÃÉ|ÉäÉSÉì
********************************************************/
class DrawLine :
    public DrawObject
{
public:
	void Update()override;
	void Draw()override;
	DrawLine(
		class GameObject* owner,
		const DrawLineDescription& drawLineDesc,
		int order = COMP_DRAWLINE);
	~DrawLine();

private:
	/*********************************************************
	* @brief	ï`âÊÇ…ïKê{Ç»óvëf
	* @details	ìÆìIämï€Ç∑ÇÈóvëf
	********************************************************/
	ID3D11Buffer* m_VertexBuffer;

public:
	/*********************************************************
	* @brief	çDÇ›ê›íËÇÃÇ≈Ç´ÇÈóvëf
	********************************************************/
	float Size;
	D3DXVECTOR4 Color;
	Collision::Line LineData;
	class Transform3D* StartTransform;
	class Transform3D* EndTransform;

	bool LocalPos;
	D3DXVECTOR3 StartPos;
	D3DXVECTOR3 EndPos;
};

