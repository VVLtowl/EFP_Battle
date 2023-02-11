#include "main.h"
#include "LookAtCamera.h"
#include "ParticleSystem.h"


ParticleSystem::ParticleSystem()
{
	//init shoot context
	{
		m_Duration = 10;
		m_FrameCount = 0;

		m_ShootTimes = 1;
		m_ShootCount = 0;

		m_FinishCount = INT_MAX;
	}

	//create polygon2DMany
	{
		m_Polygon2DMany = Polygon2DMany::Instance();
	}

	//create shoot transform object
	{
		m_Shooter = GameObjectManager::Create<TransformObject>();
	}
}

void ParticleSystem::UpdateGameObject()
{
	if (m_FinishCount <= 0)
	{
		SetState(DEAD);
		return;
	}

	//launch
	if (m_FrameCount == m_Duration)
	{
		//m_FrameCount = 0;
		m_FrameCount = 0;
		m_FinishCount--;

		//test
		//#pragma omp simd
		for (m_ShootCount = 0; m_ShootCount < m_ShootTimes; m_ShootCount++)
		{
			//test shoot
			PARTICLE* p = new PARTICLE();
			m_Particles.emplace_back(p);
			p->LifeCountDown = PARTICLE_LIFE_COUNT;
			p->Position = GetShooter()->GetWorldPosition();
			p->Scale = V3_ONE * 0.4f;
			p->Color = {
				rand() % 100 * 0.01f + 0.6f,
				rand() % 100 * 0.01f + 0.6f,
				rand() % 100 * 0.01f + 0.6f,
				1
			};


			//compute physics
			if(1)
			{
				float num = 0.05f;
				float num2 = 0.001f;
				float num3 = num2 / 700.0f;
				float veloX = (rand() % 100 - 50) / 100.0f * num;
				float veloY = num;// (rand() % 100 - 50) / 100.0f * num + num2;
				float veloZ = (rand() % 100 - 50) / 100.0f * num;
				p->Velocity = { veloX,veloY,veloZ };
				p->Acceleration = -(p->Velocity * 1 / (float)PARTICLE_LIFE_COUNT);
				p->Acceleration.y= -(p->Velocity.y * 7 / (float)PARTICLE_LIFE_COUNT);
			}
		}
	}
	else
	{
		m_FrameCount++;
	}

	//update particle
	{
		//int count = 0;//get particle amount
		for (auto p : m_Particles)
		{
			float ratio = (float)(p->LifeCountDown) / PARTICLE_LIFE_COUNT;
			//move
			p->Position += p->Velocity;
			//update scale
			p->Scale *= powf( ratio,0.25f);
			//update color
			p->Color.w = ratio;

			//set instance data
			D3DXVECTOR3 pos = p->Position;
			D3DXVECTOR3 scl = p->Scale;
			Camera* cmr = CameraManager::GetMainCamera();
			LookAtCameraMatrix lookAt;
			D3DXMATRIX world = lookAt(pos, scl, cmr);
			world *= m_Transform3D->GetWorldMatrix();//need partcle system transform
			D3DXMatrixTranspose(&world, &world);
			INSTANCE_SQUARE data(world, p->Color);
			m_Polygon2DMany->AddInstanceData(data);

			//update physics
			p->Velocity += p->Acceleration;
			if (p->Position.y <= 0)
			{
				p->Velocity.y *= -0.8f;
			}

			//count down life and check dead
			p->LifeCountDown--;
			if (p->LifeCountDown <= 0)
			{
				m_DeadParticles.emplace_back(p);
			}
		}

		//delete dead particles
		for (auto p : m_DeadParticles)
		{
			m_Particles.remove(p);
			delete p;
		}
		m_DeadParticles.clear();
	}
}

Transform3D* ParticleSystem::GetShooter()
{
	return m_Shooter->GetTransform();
}
