#pragma once

#include "DrawObject.h"
#include "Transform3D.h"
#include <string>
#include "model.h"

/*********************************************************
* @brief	3D描画対象を作るための説明
********************************************************/
struct Polygon3DDescription
{
	DrawObjectDescription DrawObjDesc;
	std::string ModelName = "asset\\model\\torus.obj";
	int ModelID = MDLID_TORUS;
};

/*********************************************************
* @brief	3Dメッシュのあるポリゴン
*			ボーンアニメ付いていない
********************************************************/
class Polygon3D :
	public DrawObject
{
public:
	Model* Model;

public:
	void Draw()override;
	Polygon3D(
		class GameObject* owner, 
		const Polygon3DDescription& polygon3DDesc,
		int order = COMP_POLYGON3D);
};



