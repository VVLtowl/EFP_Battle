#pragma once

#include "DrawObject.h"
#include <string>

/*********************************************************
* @brief	2Dï`âÊëŒè€ÇçÏÇÈÇΩÇﬂÇÃê‡ñæ
********************************************************/
struct Polygon2DDescription
{
	DrawObjectDescription DrawObjDesc;
	D3DXVECTOR2 Size = { 1,1 };
	D3DXVECTOR4 Color = { 1,1,1,1 };
	//std::string TextureName = "asset\\texture\\number.dds";//give up
	short TextureID = 0;
};


/*********************************************************
* @brief	éläpå`ÇÃÉ|ÉäÉSÉì
********************************************************/
class Polygon2D:
	public DrawObject
{
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
	D3DXVECTOR2 Size = {1,1};
	D3DXVECTOR4 Color = {1,1,1,1};
	ID3D11ShaderResourceView* Texture;

public:
	void Draw()override;
	Polygon2D(
		class GameObject* owner,
		const Polygon2DDescription& polygon2DDesc,
		int order = COMP_POLYGON2D);
	~Polygon2D();
};

