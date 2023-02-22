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
	CAMERA m_CameraData;
	class LookAt* m_CmrLookAt;
	bool m_IsOrtho;
	float m_Fovy;
	float m_Width;
	float m_Height;
	float m_ZNear;
	float m_ZFar;

	ID3D11DepthStencilView* m_MainDSV;
	ID3D11Texture2D* m_MainTex;
	ID3D11RenderTargetView* m_MainRT;
	ID3D11ShaderResourceView* m_MainSRV;

public:
	Camera(
		class GameObject* owner, 
		CameraDescription cmrDesc, 
		int order = COMP_CAMERA);
	~Camera();
	void Update()override;
	void Draw();
};

