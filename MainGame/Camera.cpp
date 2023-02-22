#include "main.h"
#include "renderer.h"
#include "Transform3D.h"
#include "GameObject.h"
#include "Camera.h"

#include "LookAt.h"

Camera::Camera(class GameObject* owner, CameraDescription cmrDesc, int order):
	Component(owner,order)
{
	m_CameraData = cmrDesc.CameraData;
	//LookAt = cmrDesc.LookAtTransform;
	m_IsOrtho = cmrDesc.IsOrtho;
	m_Fovy = cmrDesc.Fovy;
	m_Width = cmrDesc.Width;
	m_Height = cmrDesc.Height;
	m_ZNear = cmrDesc.ZNear;
	m_ZFar = cmrDesc.ZFar;

	//add lookAt to owner
	{
		m_CmrLookAt = new LookAt(m_Owner);
	}

	//��ʂ��L�^
	{
		ID3D11Device* device = Renderer::GetDevice();

		//��ʃo�b�t�@�쐬
		D3D11_TEXTURE2D_DESC td{};
		td.Width = SCREEN_WIDTH;
		td.Height = SCREEN_HEIGHT;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R32_TYPELESS;
		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		td.SampleDesc.Count = 1;// swapChainDesc.SampleDesc;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;
		device->CreateTexture2D(&td, NULL, &m_MainTex);

		// �����_�[�^�[�Q�b�g�r���[�쐬
		D3D11_RENDER_TARGET_VIEW_DESC rtvd;
		rtvd.Format = td.Format;
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvd.Texture2D.MipSlice = 0;
		device->CreateRenderTargetView(
			m_MainTex,
			&rtvd,
			&m_MainRT
		);

		ID3D11Texture2D* depthStencileTex = NULL;
		D3D11_TEXTURE2D_DESC dstd{};
		dstd.Width = SCREEN_WIDTH;
		dstd.Height = SCREEN_HEIGHT;
		dstd.MipLevels = 1;
		dstd.ArraySize = 1;
		dstd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dstd.SampleDesc.Count = 1;// swapChainDesc.SampleDesc;
		dstd.Usage = D3D11_USAGE_DEFAULT;
		dstd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dstd.CPUAccessFlags = 0;
		dstd.MiscFlags = 0;
		device->CreateTexture2D(
			&dstd,
			NULL,
			&depthStencileTex
		);
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd{};
		dsvd.Format = dstd.Format;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Flags = 0;
		device->CreateDepthStencilView(
			depthStencileTex,
			&dsvd,
			&m_MainDSV
		);

		//�V�F�[�_�[���\�[�X�r���[�쐬
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
		srvd.Format = DXGI_FORMAT_R32_FLOAT;//�s�N�Z���t�H�[�}�b�g��32BitFLOAT�^
		srvd.Format = td.Format;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(
			m_MainTex,
			&srvd,
			&m_MainSRV
		);
	}
}

void Camera::Update()
{
	Transform3D* trans = GetGameObject()->GetTransform();
	D3DXVECTOR3 pos = trans->GetWorldPosition();
	D3DXVECTOR3 up = V3_AXIS_Y;
	D3DXVECTOR3 forwardPos = pos + trans->GetForward() * 1.0f;
	D3DXVECTOR3 lookAtPos =
		(m_CmrLookAt->m_TargetTransform == nullptr ?
			forwardPos :
			m_CmrLookAt->m_TargetTransform->GetWorldPosition());
	D3DXVECTOR3 dir = (lookAtPos - pos);
	D3DXVec3Normalize(&dir, &dir);

	//test
	//pos -= dir * 4;

	m_CameraData.Direction = D3DXVECTOR4(dir, 0);
	m_CameraData.Position = D3DXVECTOR4(pos, 0);
	D3DXMatrixLookAtLH(
		&m_CameraData.ViewMatrix,
		&pos,
		&lookAtPos,
		&up
	);


	if (m_IsOrtho==true)
	{
		//orthographic
		D3DXMatrixOrthoLH(
			&m_CameraData.ProjectionMatrix,
			m_Width,//w
			m_Height,//h
			m_ZNear,//near 
			m_ZFar//far
		);
	}
	else
	{
		//Perspective
		D3DXMATRIX* cmrProjection = &m_CameraData.ProjectionMatrix;
		float aspect = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		float radian = m_Fovy * PI / 180.0f;
		D3DXMatrixPerspectiveFovLH(
			&m_CameraData.ProjectionMatrix,
			radian,
			aspect,
			m_ZNear,
			m_ZFar
		);
	}

}

Camera::~Camera()
{
	m_MainDSV->Release();
	m_MainRT->Release();
	m_MainSRV->Release();
	m_MainTex->Release();
}

void Camera::Draw()
{
	Renderer::Begin(this);
	CAMERA camera = m_CameraData;
	Renderer::SetViewMatrix(&camera.ViewMatrix);
	Renderer::SetProjectionMatrix(&camera.ProjectionMatrix);

	DrawManager::DrawWorldObject();
}
