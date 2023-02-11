#include "main.h"
#include "MainLight.h"

#include "imgui/imgui.h"

MainLight::MainLight():
	m_TestBlockID(0),
	m_CanMove(false)
{
	//set name
	//SetName("MainLight");

	//set position
	m_Transform3D->SetPosition(5, 5, -5);

	//create light component
	LightDescription desc;
	desc.LightData.Diffuse = { 1.0f,1.0f,1,1 };
	desc.LightData.Ambient = { 3.0f,3.0f,3.23f,0.1f };
	desc.Width = 32;
	desc.Height = 18;
	desc.ZNear = 1;
	desc.ZFar = 100;
	m_MyLight = new MyLight(this, desc);

	//set main light
	LightManager::SetMainLight(m_MyLight);

	//test add edit block
	if(0)
	{
		m_TestBlockID = 3;
		m_CanMove = false;
		DebugInfo::TestBlocks.emplace(
			m_TestBlockID,
			[this]()
			{
				ImGui::Begin("Edit Main Light");
				ImGui::SliderFloat("Fovy", &(m_MyLight->Fovy), 0.1f, 45.0f);
				ImGui::Checkbox("IsOrtho", &(m_MyLight->IsOrtho));
				ImGui::Checkbox("Move", &(m_CanMove));

				D3DXVECTOR3 v3 = m_Transform3D->GetWorldPosition();
				ImGui::InputFloat3("pos", v3);
				m_Transform3D->SetPosition(v3);
				//ImGui::Text("scl: x:%.2lf y:%.2lf z:%.2lf",
				//	m_Transform3D->GetScale().x,
				//	m_Transform3D->GetScale().y,
				//	m_Transform3D->GetScale().z);
				ImGui::End();
			}
		);
	}
}

MainLight::~MainLight()
{
	LightManager::SetMainLight(nullptr);
}

void MainLight::UpdateGameObject()
{

	//update render main light data
	//Renderer::SetLight(m_MyLight->LightData);
}

