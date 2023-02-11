#include "main.h"
#include "EffectParticle.h"

#include "LookAtCamera.h"

const float EFFECTPARTICLE_LIFETIME = 120;

EffectParticle::EffectParticle()
{
	//other init
	{
		m_CountDown = EFFECTPARTICLE_LIFETIME;

		//compute physics
		float num = 0.02f;
		float num2 = 0.01f;
		float num3 = num2 / 700.0f;
		float veloX = (rand() % 100 - 50) / 100.0f * num;
		float veloY = (rand() % 100 - 50) / 100.0f * num + num2;
		float veloZ = (rand() % 100 - 50) / 100.0f * num;
		m_Velocity = { veloX,veloY,veloZ };
		m_Acceleration = { 0,-(rand() % 100) / 100.0f * num2,0 };
	}

	//create polygon2D
	{
		Polygon2DDescription desc;
		//desc.DrawObjDesc.Layer = LAYER_WORLD_ALPHA;
		desc.DrawObjDesc.Layer = LAYER_WORLD_PARTICLE;
		desc.DrawObjDesc.Shader = SHADER_UNLIT;
		desc.TextureID = TEXID_CIRCLE_0;
		desc.Size = { 1,1 };
		desc.DrawObjDesc.DepthEnable = false;
		desc.Color = { 1,1,0.3f,1 };
		m_Polygon2D = new Polygon2D(this, desc);
		//pause polygon2D
		m_Polygon2D->SetState(Component::PAUSED);
	}
	
	//create look at camera
	{
		Camera* cmr = CameraManager::GetMainCamera();
		new LookAtCamera(this, cmr);
	}
}

void EffectParticle::UpdateGameObject()
{	
	//test
	//move
	D3DXVECTOR3 pos = m_Transform3D->GetPosition();
	pos += m_Velocity;
	m_Transform3D->SetPosition(pos);
	//update physics
	m_Velocity += m_Acceleration;


	m_CountDown--;
	//check destroy
	if (m_CountDown < 0)
	{
		SetState(DEAD);
	}

	//test
	float ratio = m_CountDown / EFFECTPARTICLE_LIFETIME;
	//update size
	m_Polygon2D->Size = V2_ONE * 0.3f * ratio;// V2_ONE* m_CountDown / 120.0f * 0.1f;
	//update alpha
	m_Polygon2D->Color.w = ratio;

	m_Polygon2D->SetState(Component::ACTIVE);
	//test
	//look at camera
	//D3DXMATRIX view =CameraManager::GetMainCamera()->CameraData.ViewMatrix;
	//m_Transform3D->LookAtView(view);
}

void EffectParticle::SetUpPhysics(const D3DXVECTOR3& startPos, const D3DXVECTOR3& dir)
{
	m_Transform3D->SetPosition(startPos);
	D3DXVECTOR3 unitDir;
	D3DXVec3Normalize(&unitDir, &dir);
	//m_Velocity += unitDir * 0.00001f;

	m_Velocity *= 0.1f;
	//m_Velocity = V3_ZERO;
	m_Acceleration = {0,0,0};
}
