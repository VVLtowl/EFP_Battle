#include "main.h"
#include "Polygon2DMany.h"

DrawObjectDescription g_Polygon2DManyDesc =
{
	LAYER_WORLD_ALPHA,
	SHADER_UNLIT_INSTANCE,
	false,
};

Polygon2DMany::Polygon2DMany():
	DrawObject(nullptr, g_Polygon2DManyDesc)
{
	//get square vertex
	m_Buffer[VERTEX] = DrawManager::SquareVertexBuffer;

	//create instance
	m_Buffer[INSTANCE] = DrawManager::SquareInstanceBuffer;
}

Polygon2DMany::~Polygon2DMany()
{
	m_Buffer[VERTEX] = nullptr;
	m_Buffer[INSTANCE] = nullptr;
}

void Polygon2DMany::AddInstanceData(const INSTANCE_SQUARE& data)
{
	m_InstanceDatas[m_InstanceCount].World = data.World;
	m_InstanceDatas[m_InstanceCount].Color = data.Color;
	m_InstanceCount++;

	if (m_InstanceCount > INSTANCE_SQUARE_MAX)
	{
		m_InstanceCount = INSTANCE_SQUARE_MAX;
		DebugInfo::Print("warning : particle count max");
	}
}

void Polygon2DMany::EndFrame()
{
	m_InstanceCount = 0;
}

void Polygon2DMany::Draw()
{
	ID3D11DeviceContext* dc = Renderer::GetDeviceContext();

	//anime pic slide and 
	if (1)
	{
		//update vertex 
		if(0)
		{
			float x = 0;
			float y = 0;
			float w = 1;
			float h = 1;

			float width = 1;
			float height = 1;
			float leftTopX = -width / 2.0f;
			float leftTopY = height / 2.0f;
			float z = 0;

			D3D11_MAPPED_SUBRESOURCE msr;
			dc->Map(
				m_Buffer[VERTEX],
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&msr);

			VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

			vertex[0].Position = { leftTopX,leftTopY,z };
			vertex[0].Normal = { 0,0,-1 };
			vertex[0].Diffuse = V4_ONE;
			vertex[0].TexCoord = { x,y };

			vertex[1].Position = { leftTopX + width,leftTopY,z };
			vertex[1].Normal = { 0,0,-1 };
			vertex[1].Diffuse = V4_ONE;
			vertex[1].TexCoord = { x + w,y };

			vertex[2].Position = { leftTopX,leftTopY - height,z };
			vertex[2].Normal = { 0,0,-1 };
			vertex[2].Diffuse = V4_ONE;
			vertex[2].TexCoord = { x,y + h };

			vertex[3].Position = { leftTopX + width,leftTopY - height,z };
			vertex[3].Normal = { 0,0,-1 };
			vertex[3].Diffuse = V4_ONE;
			vertex[3].TexCoord = { x + w,y + h };

			dc->Unmap(m_Buffer[VERTEX], 0);
		}
	
		//update instance
		if(1)
		{
			D3D11_MAPPED_SUBRESOURCE msr;
			dc->Map(
				m_Buffer[INSTANCE],
				0,//0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				&msr);

			INSTANCE_SQUARE* instance = (INSTANCE_SQUARE*)msr.pData;
			for (int i = 0; i < m_InstanceCount; i++)
			{
				instance[i].World = m_InstanceDatas[i].World;
				instance[i].Color = m_InstanceDatas[i].Color;
			}

			dc->Unmap(m_Buffer[INSTANCE], 0);
		}
	}

	dc->IASetInputLayout(m_VertexLayout);
	dc->VSSetShader(m_VertexShader, NULL, 0);
	dc->PSSetShader(m_PixelShader, NULL, 0);

	//Renderer::SetWorldViewProjection2D();
	//ワールドマトリクス設定
	//D3DXMATRIX world = GetGameObject()->GetTransform()->GetWorldMatrix();
	D3DXMATRIX world = MTX_NOTRANSFORM;
	Renderer::SetWorldMatrix(&world);

	UINT stride[2] = { sizeof(VERTEX_3D),sizeof(INSTANCE_SQUARE) };
	UINT offset[2] = {0};
	dc->IASetVertexBuffers(
		0,
		2,
		m_Buffer,
		stride,
		offset);

	dc->VSSetShaderResources(1, 1, &DrawManager::StructuredBufferSRV);

	//UINT stride = sizeof(VERTEX_3D);
	//UINT offset = 0;
	//dc->IASetVertexBuffers(
	//	0,
	//	1,
	//	&m_Buffer[VERTEX],
	//	&stride,
	//	&offset);

	//マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { V4_ONE.x,V4_ONE.y,V4_ONE.z,V4_ONE.w };
	//Renderer::SetMaterial(material);



	//シャドウバッファテクスチャを１番へセット
	ID3D11ShaderResourceView* shadowDepthTexture = Renderer::GetShadowDepthTexture();
	dc->PSSetShaderResources(1, 1, &shadowDepthTexture);
	dc->PSSetShaderResources(0, 1, &DrawManager::Textures[TEXID_CIRCLE_0]);


	Renderer::SetDepthEnable(m_DepthEnable);
	//Renderer::SetAlphaCoverageEnable(true);

	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->DrawInstanced(4, m_InstanceCount, 0, 0);
	//m_InstanceCount = 0;//reset count after draw
	//dc->Draw(4, 0);
}