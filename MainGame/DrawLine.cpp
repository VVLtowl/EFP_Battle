#include "main.h"
#include "DrawLine.h"


DrawLine::DrawLine(GameObject* owner, const DrawLineDescription& drawLineDesc, int order):
	DrawObject(owner, drawLineDesc.DrawObjDesc,order),
	Size(drawLineDesc.Size),
	Color(drawLineDesc.Color),
	StartTransform(drawLineDesc.StartTransform),
	EndTransform(drawLineDesc.EndTransform),
	StartPos(drawLineDesc.StartPos),
	EndPos(drawLineDesc.EndPos),
	LocalPos(drawLineDesc.LocalPos)
{
	m_VertexBuffer = DrawManager::SquareVertexBuffer;
}

DrawLine::~DrawLine()
{
	m_VertexBuffer = nullptr;
}


void DrawLine::Update()
{
	//update line data

	if (StartTransform != nullptr && EndTransform != nullptr)
	{
		LineData.Start = StartTransform->GetWorldPosition();
		LineData.End = EndTransform->GetWorldPosition();
		return;
	}

	LineData.Start = StartPos;
	LineData.End = EndPos;
}

void DrawLine::Draw()
{
	ID3D11DeviceContext* dc = Renderer::GetDeviceContext();

	//anime pic slide
	if (1)
	{
		//================================
		//      pt1______________________pt3
		//  start |                      | end
		//        |______________________|
		//      pt0                      pt2
		//================================

		float x = 0;
		float y = 0;
		float w = 1;
		float h = 1;

		D3DXVECTOR3 start = LineData.Start;
		D3DXVECTOR3 end = LineData.End;

		if (LocalPos)
		{
			D3DXMATRIX world = m_Owner->GetTransform()->GetWorldMatrix();
			D3DXVec3TransformCoord(&start, &start, &world);
			D3DXVec3TransformCoord(&end, &end, &world);
		}

		D3DXVECTOR3 dir = end - start;
		D3DXVec3Normalize(&dir, &dir);
		D3DXVECTOR4 cmrDir = CameraManager::GetMainCamera()->CameraData.Direction;
		D3DXVECTOR3 rotateAxis = { -cmrDir.x,-cmrDir.y,-cmrDir.z };
		float lengthOnRotateAxis = D3DXVec3Dot(&rotateAxis, &dir);
		D3DXVECTOR3 dirOnCmrViewPlane = dir - lengthOnRotateAxis * rotateAxis;
		D3DXVec3Normalize(&dirOnCmrViewPlane, &dirOnCmrViewPlane);

		D3DXMATRIX rotateTransform,rotate,scale,translate;
		D3DXMatrixScaling(&scale, 1, 1, 1);
		D3DXMatrixRotationAxis(&rotate, &rotateAxis, PI / 2.0f);
		D3DXMatrixTranslation(&translate, 0, 0, 0);
		rotateTransform = scale * rotate * translate;

		D3DXVECTOR3 verticalDir;
		D3DXVec3TransformCoord(&verticalDir, &dirOnCmrViewPlane, &rotate);

		D3DXVECTOR3 pt0 = start - Size / 2.0f * verticalDir;
		D3DXVECTOR3 pt1 = start + Size / 2.0f * verticalDir;
		D3DXVECTOR3 pt2 = end - Size / 2.0f * verticalDir;
		D3DXVECTOR3 pt3 = end + Size / 2.0f * verticalDir;

		//D3DXMATRIX invRot;
		//D3DXQUATERNION quat = m_Owner->GetTransform()->GetRotation();
		//D3DXMatrixRotationQuaternion(&invRot, &quat);
		//D3DXMatrixInverse(&invRot,0, &invRot);
		//D3DXVec3TransformCoord(&pt0, &pt0, &invRot);
		//D3DXVec3TransformCoord(&pt1, &pt1, &invRot);
		//D3DXVec3TransformCoord(&pt2, &pt2, &invRot);
		//D3DXVec3TransformCoord(&pt3, &pt3, &invRot);

		D3D11_MAPPED_SUBRESOURCE msr;
		dc->Map(
			m_VertexBuffer,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		vertex[0].Position = pt0;
		vertex[0].Normal = rotateAxis;
		vertex[0].Diffuse = Color;
		vertex[0].TexCoord = { x,y };

		vertex[1].Position = pt1;
		vertex[1].Normal = rotateAxis;
		vertex[1].Diffuse = Color;
		vertex[1].TexCoord = { x + w,y };

		vertex[2].Position = pt2;
		vertex[2].Normal = rotateAxis;
		vertex[2].Diffuse = Color;
		vertex[2].TexCoord = { x,y + h };

		vertex[3].Position = pt3;
		vertex[3].Normal = rotateAxis;
		vertex[3].Diffuse = Color;
		vertex[3].TexCoord = { x + w,y + h };

		dc->Unmap(m_VertexBuffer, 0);
	}

	dc->IASetInputLayout(m_VertexLayout);
	dc->VSSetShader(m_VertexShader, NULL, 0);
	dc->PSSetShader(m_PixelShader, NULL, 0);

	Renderer::SwitchWVP2D3D(true);
	//普通のワールドマトリクス設定
	D3DXMATRIX world = MTX_NOTRANSFORM;
	Renderer::SetWorldMatrix(&world);

	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	dc->IASetVertexBuffers(
		0,
		1,
		&m_VertexBuffer,
		&stride,
		&offset);

	//マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1,1,1,1 };
	Renderer::SetMaterial(material);



	//シャドウバッファテクスチャを１番へセット
	//ID3D11ShaderResourceView* shadowDepthTexture = Renderer::GetShadowDepthTexture();
	//dc->PSSetShaderResources(1, 1, &shadowDepthTexture);
	dc->PSSetShaderResources(0, 1, &DrawManager::Textures[TEXID_FIELD004]);

	Renderer::SetDepthEnable(m_DepthEnable);

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->Draw(4, 0);

	//Renderer::SetAlphaCoverageEnable(false);
}
