#include "main.h"
#include "renderer.h"
#include "resource.h"
#include "drawManager.h"
#include "GameObject.h"
#include "Polygon3D.h"

void Polygon3D::Draw()
{
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);
	
	Renderer::SwitchWVP2D3D(true);
	//ワールドマトリクス設定
	Transform3D* trans = GetGameObject()->GetTransform();
	D3DXMATRIX world = trans->GetWorldMatrix();
	Renderer::SetWorldMatrix(&world);

	//シャドウバッファテクスチャを１番へセット
	ID3D11ShaderResourceView* shadowDepthTexture = Renderer::GetShadowDepthTexture();
	Renderer::GetDeviceContext()->PSSetShaderResources(1, 1, &shadowDepthTexture);

	Renderer::SetDepthEnable(m_DepthEnable);
	Model->Draw();
}

Polygon3D::Polygon3D(
	GameObject* owner, 
	const Polygon3DDescription& polygon3DDesc, 
	int order):
	DrawObject(owner,polygon3DDesc.DrawObjDesc,order)
{
	Model = DrawManager::Models[polygon3DDesc.ModelID];
}

