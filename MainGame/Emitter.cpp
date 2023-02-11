#include "main.h"
#include "EffectParticle.h"


Emitter::Emitter()
{
	 m_Duration = 1;
	 m_FrameCount = 0;

	 m_ShootTimes = 1;
	 m_ShootCount = 0;

	 m_FinishCount = 1000;
}

void Emitter::UpdateGameObject()
{
	if (m_FinishCount < 0)
	{
		return;
	}
	//test
	if (m_FrameCount == m_Duration)
	{
		//m_FrameCount = 0;
		//m_FinishCount--;
		m_FrameCount = 0;
		m_ShootCount = 0;

		//test
#pragma omp simd
		for (; m_ShootCount < m_ShootTimes; m_ShootCount++)
		{
			EffectParticle* ep = GameObjectManager::Create<EffectParticle>();

			D3DXVECTOR3 pos = m_Transform3D->GetWorldPosition();
			pos += m_Transform3D->GetUp() *0.1f * (float)m_ShootCount;
			ep->SetUpPhysics(pos, m_Transform3D->GetUp());
		}
	}
	else
	{
		m_FrameCount++;
	}
}
