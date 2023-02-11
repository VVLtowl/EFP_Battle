#include "main.h"
#include "renderer.h"
#include "resource.h"
#include "drawManager.h"
#include "GameObject.h"
#include "PolygonUI.h"

void PolygonUI::Draw()
{	
	ID3D11DeviceContext* dc = Renderer::GetDeviceContext();

	//anime pic slide
	{
		float x = Texcoord.x;
		float y = Texcoord.y;
		float w = Texcoord.z;
		float h = Texcoord.w;

		D3DXVECTOR3 pos = m_Owner->GetTransform()->GetWorldPosition();
		D3DXVECTOR3 scl = m_Owner->GetTransform()->GetWorldScale();
		float width = Size.x * scl.x;
		float height = Size.y * scl.y;
		float leftTopX = -width / 2.0f + pos.x;
		float leftTopY = -height / 2.0f + pos.y;
		float z = 0;

		D3D11_MAPPED_SUBRESOURCE msr;
		dc->Map(
			m_VertexBuffer,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&msr
		);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		vertex[0].Position = { leftTopX,leftTopY,z };
		vertex[0].Normal = { 0,0,-1 };
		vertex[0].Diffuse = Color;
		vertex[0].TexCoord = { x,y };

		vertex[1].Position = { leftTopX + width,leftTopY,z };
		vertex[1].Normal = { 0,0,-1 };
		vertex[1].Diffuse = Color;
		vertex[1].TexCoord = { x + w,y };

		vertex[2].Position = { leftTopX,leftTopY + height,z };
		vertex[2].Normal = { 0,0,-1 };
		vertex[2].Diffuse = Color;
		vertex[2].TexCoord = { x,y + h };

		vertex[3].Position = { leftTopX + width,leftTopY + height,z };
		vertex[3].Normal = { 0,0,-1 };
		vertex[3].Diffuse = Color;
		vertex[3].TexCoord = { x + w,y + h };

		dc->Unmap(m_VertexBuffer, 0);
	}

	dc->IASetInputLayout(m_VertexLayout);
	dc->VSSetShader(m_VertexShader, NULL, 0);
	dc->PSSetShader(m_PixelShader, NULL, 0);

	//Renderer::SetWorldViewProjection2D();
	Renderer::SwitchWVP2D3D(false);

	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	dc->PSSetShaderResources(0, 1, &Texture);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->Draw(4, 0);
}

PolygonUI::PolygonUI(GameObject* owner, const PolygonUIDescription& polygonUIDesc, int order):
	DrawObject(owner,polygonUIDesc.DrawObjDesc,order)
{
	Size = polygonUIDesc.Size;
	Color = polygonUIDesc.Color;
	Texture = DrawManager::Textures[polygonUIDesc.TextureID];

	m_VertexBuffer = DrawManager::SquareVertexBuffer;
}

PolygonUI::~PolygonUI()
{
	m_VertexBuffer = nullptr;
}
