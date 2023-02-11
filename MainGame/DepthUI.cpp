#include "main.h"
#include "DepthUI.h"

#include "imgui/imgui.h"

DepthUI::DepthUI():
	m_Show(false),
	m_TestBlockID(1)
{
	//create polygonUI
	PolygonUIDescription desc;
	desc.Size = D3DXVECTOR2(1280, 720)*0.25f;
	m_PolygonUI = new PolygonUI(this, desc);

	//set render shadow depth texture
	m_PolygonUI->Texture = Renderer::GetShadowDepthTexture();//Renderer::m_MainSRV;// 

	//set pos
	m_Transform3D->SetPosition(m_PolygonUI->Size.x / 2.0f, m_PolygonUI->Size.y / 2.0f, 0);

	//test block
	{
		m_TestBlockID = 1;
		DebugInfo::TestBlocks.emplace(
			TESTBLOCKID_DEPTHUI,
			[this]()
			{
				ImGui::Begin("Edit Depth Texture");
				ImGui::Checkbox("Show", &m_Show);
				ImGui::SliderFloat("ZFar", 
					&(LightManager::GetMainLight()->ZFar), 
					LightManager::GetMainLight()->ZNear, 
					100.0f);
				//ImGui::Checkbox("Move", &(m_CanMoveAni));
				//ImGui::InputFloat("MoveSpeed", &m_MoveSpeed);
				//ImGui::Text("tanHalfFovy: %.3lf", tanf(m_Camera->Fovy / 2 * PI / 180));
				//ImGui::Text("pos: x:%.2lf y:%.2lf z:%.2lf",
				//	m_Transform3D->GetPosition().x,
				//	m_Transform3D->GetPosition().y,
				//	m_Transform3D->GetPosition().z);
				//ImGui::Text("scl: x:%.2lf y:%.2lf z:%.2lf",
				//	m_Transform3D->GetScale().x,
				//	m_Transform3D->GetScale().y,
				//	m_Transform3D->GetScale().z);
				ImGui::End();
			}
		);
	}
}

DepthUI::~DepthUI()
{
	DebugInfo::TestBlocks.erase(TESTBLOCKID_DEPTHUI);
}

void DepthUI::UpdateGameObject()
{
	if (m_Show)
	{
		m_PolygonUI->SetState(Component::ACTIVE);
	}
	else
	{
		m_PolygonUI->SetState(Component::PAUSED);
	}
}