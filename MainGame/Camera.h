#pragma once
#include "Component.h"

/*********************************************************
* @brief	ÉJÉÅÉâëŒè€ÇçÏÇÈÇΩÇﬂÇÃê‡ñæ
********************************************************/
struct CameraDescription
{
	CAMERA CameraData;
	//class Transform3D* LookAtTransform = nullptr;
	bool IsOrtho = true;
	float Fovy = 25.0f;
	float Width = 16;
	float Height = 9;
	float ZNear = 1.0f;
	float ZFar = 100.0f;
};


class Camera :
	public Component
{
public:
	CAMERA CameraData;
	class LookAt* CmrLookAt;
	bool IsOrtho;
	float Fovy;
	float Width;
	float Height;
	float ZNear;
	float ZFar;

	ID3D11DepthStencilView* MainDSV;
	ID3D11Texture2D* MainTex;
	ID3D11RenderTargetView* MainRT;
	ID3D11ShaderResourceView* MainSRV;

public:
	Camera(
		class GameObject* owner, 
		CameraDescription cmrDesc, 
		int order = COMP_CAMERA);
	~Camera();
	void Update()override;
	void Draw();
};

