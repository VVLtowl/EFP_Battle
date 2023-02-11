#pragma once
#include "Component.h"

/*********************************************************
* @brief	ÉâÉCÉgëŒè€ÇçÏÇÈÇΩÇﬂÇÃê‡ñæ
********************************************************/
struct LightDescription
{
	LIGHT LightData;
	bool IsOrtho = true;
	float Fovy = 25.0f;
	float Width = 16;
	float Height = 9;
	float ZNear = 1.0f;
	float ZFar = 1000.0f;
	float Angle = 20.0f;//(0,90)
};

class MyLight:
	public Component
{
public:
	LIGHT LightData;
	class LookAt* LightLookAt;
	bool IsOrtho;
	float Fovy;
	float Width;
	float Height;
	float ZNear;
	float ZFar;
	float Angle;

public:
	void Update()override;
	void Draw();
	MyLight(
		class GameObject* owner,
		LightDescription litDesc, 
		int order = COMP_MYLIGHT);
};

