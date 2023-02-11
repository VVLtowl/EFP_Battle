#include "main.h"
#include "renderer.h"
#include "resource.h"
#include "drawManager.h"
#include "GameObject.h"
#include "Polygon2D.h"

Polygon2D::Polygon2D(GameObject* owner, const Polygon2DDescription& polygon2DDesc, int order) :
	DrawObject(owner, polygon2DDesc.DrawObjDesc, order)
{
	Size = polygon2DDesc.Size;
	Color = polygon2DDesc.Color;
	Texture = DrawManager::Textures[polygon2DDesc.TextureID];

	//頂点バッファ生成
#if 0
	if (m_VertexBuffer == nullptr)
	{
		//四角プリゴン
		VERTEX_3D vertex[4];

		//頂点バッファ生成
		D3D11_BUFFER_DESC bd{};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * 4;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		D3D11_SUBRESOURCE_DATA sd{};
		sd.pSysMem = vertex;
		Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_VertexBuffer);
	}
#else
	m_VertexBuffer = DrawManager::SquareVertexBuffer;
#endif
}


Polygon2D::~Polygon2D()
{
	m_VertexBuffer = nullptr;
}

void Polygon2D::Draw()
{
	ID3D11DeviceContext* dc = Renderer::GetDeviceContext();

	//anime pic slide
	if (1)
	{
		float x = Texcoord.x;
		float y = Texcoord.y;
		float w = Texcoord.z;
		float h = Texcoord.w;

		float width = Size.x;
		float height = Size.y;
		float leftTopX = -width / 2.0f;
		float leftTopY = height / 2.0f;
		float z = 0;

		D3D11_MAPPED_SUBRESOURCE msr;
		dc->Map(
			m_VertexBuffer,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		vertex[0].Position = { leftTopX,leftTopY,z };
		vertex[0].Normal = { 0,0,-1 };
		vertex[0].Diffuse = Color;
		vertex[0].TexCoord = { x,y };

		vertex[1].Position = { leftTopX + width,leftTopY,z };
		vertex[1].Normal = { 0,0,-1 };
		vertex[1].Diffuse = Color;
		vertex[1].TexCoord = { x + w,y };

		vertex[2].Position = { leftTopX,leftTopY - height,z };
		vertex[2].Normal = { 0,0,-1 };
		vertex[2].Diffuse = Color;
		vertex[2].TexCoord = { x,y + h };

		vertex[3].Position = { leftTopX + width,leftTopY - height,z };
		vertex[3].Normal = { 0,0,-1 };
		vertex[3].Diffuse = Color;
		vertex[3].TexCoord = { x + w,y + h };

		dc->Unmap(m_VertexBuffer, 0);
	}

	dc->IASetInputLayout(m_VertexLayout);
	dc->VSSetShader(m_VertexShader, NULL, 0);
	dc->PSSetShader(m_PixelShader, NULL, 0);

	//Renderer::SetWorldViewProjection2D();
	Renderer::SwitchWVP2D3D(true);
	//ワールドマトリクス設定
	D3DXMATRIX world = GetGameObject()->GetTransform()->GetWorldMatrix();
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
	material.Diffuse = { Color.x,Color.y,Color.z,Color.w };
	Renderer::SetMaterial(material);



	//シャドウバッファテクスチャを１番へセット
	ID3D11ShaderResourceView* shadowDepthTexture = Renderer::GetShadowDepthTexture();
	dc->PSSetShaderResources(1, 1, &shadowDepthTexture);
	dc->PSSetShaderResources(0, 1, &Texture);


	Renderer::SetDepthEnable(m_DepthEnable);

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->Draw(4, 0);
}
