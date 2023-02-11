#include "main.h"
#include "LookAt.h"
#include "TransformAnime.h"
#include "MainCamera.h"

#include "imgui/imgui.h"
#include "MouseMove.h"

MainCamera::MainCamera():
	m_TestBlockID(0),
	m_CanMove(false),
	m_MoveSpeedMouse(20),
	m_MoveSpeedKeyboard(0.5f)
{
	//set name
	//SetName("MainCamera");

	//create camera component
	{
		CameraDescription desc;
		desc.IsOrtho = true;
		desc.ZNear = 1;
		desc.ZFar = 100;
		m_Camera = new Camera(this, desc);
	}

	//set main camera
	CameraManager::SetMainCamera(m_Camera);

	//create LookAt
	//m_LookAt = new LookAt(this);

	//create camera polygon
#if 0
	Polygon3DDescription polyDesc;
	polyDesc.DrawObjDesc.Shader = SHADER_UNLIT;
	//polyDesc.DrawObjDesc.DepthEnable = false;
	polyDesc.ModelID = MDLID_CUBE;
	m_Polygon3D = new Polygon3D(this, polyDesc);
#endif

	//transform init
	m_Transform3D->SetPosition(10, 10, -10);


	//test add edit block
	{
		m_CanMove = false;
		DebugInfo::TestBlocks.emplace(
			TESTBLOCKID_MAINCAMERA,
			[this]()
			{
				ImGui::Begin("Edit Main Camera");
				ImGui::SliderFloat("Fovy", &(m_Camera->Fovy), 0.1f, 45.0f);
				ImGui::Checkbox("IsOrtho", &(m_Camera->IsOrtho));
				ImGui::Checkbox("Move", &(m_CanMove));
				ImGui::InputFloat("MoveSpeed-Keyboard", &m_MoveSpeedKeyboard);
				ImGui::InputFloat("MoveSpeed-Mouse", &m_MoveSpeedMouse);

				D3DXVECTOR3 pos = m_Transform3D->GetWorldPosition();
				ImGui::InputFloat3("pos", pos);
				m_Transform3D->SetPosition(pos);
				ImGui::Text("right: x:%lf\n y:%lf\n z:%lf\n",
					m_Transform3D->GetRight().x,
					m_Transform3D->GetRight().y,
					m_Transform3D->GetRight().z);
				ImGui::End();
			}
		);
	}
}

MainCamera::~MainCamera()
{
	CameraManager::SetMainCamera(nullptr);
	DebugInfo::TestBlocks.erase(TESTBLOCKID_MAINCAMERA);
}

void MainCamera::UpdateGameObject()
{
	//test move camera
	if (m_CanMove)
	{
		D3DXVECTOR3 pos = m_Transform3D->GetPosition();
		D3DXVECTOR3 scl = m_Transform3D->GetScale();
		D3DXQUATERNION rot = m_Transform3D->GetRotation();

		const D3DXVECTOR3& forward = m_Transform3D->GetForward();
		const D3DXVECTOR3& right = m_Transform3D->GetRight();
		const D3DXVECTOR3& up= m_Transform3D->GetUp();
		
		//keyboard
		{
			if (Input::Keyboard.IsKeyPress(KEY_A))
			{
				pos += -right * m_MoveSpeedKeyboard;
			}
			if (Input::Keyboard.IsKeyPress(KEY_D))
			{
				pos += right * m_MoveSpeedKeyboard;
			}

			if (Input::Keyboard.IsKeyPress(KEY_W))
			{
				pos += forward * m_MoveSpeedKeyboard;
			}
			if (Input::Keyboard.IsKeyPress(KEY_S))
			{
				pos += -forward * m_MoveSpeedKeyboard;
			}

			if (Input::Keyboard.IsKeyPress(KEY_Z))
			{
				pos += -up * m_MoveSpeedKeyboard;
			}
			if (Input::Keyboard.IsKeyPress(KEY_X))
			{
				pos += up * m_MoveSpeedKeyboard;
			}
		}

		//mouse
		{
			const D3DXVECTOR2& move = MouseMove::Instance()->GetSpeed();
			if (Input::Mouse.IsButtonPress(MOUSE_LEFT))
			{
				pos += (right * move.x - forward * move.y) * m_MoveSpeedMouse;
			}
			if (Input::Mouse.IsButtonPress(MOUSE_MID))
			{
				pos += (right * move.x - up * move.y) * m_MoveSpeedMouse;
			}
			if (Input::Mouse.IsButtonPress(MOUSE_RIGHT))
			{
				D3DXQUATERNION rotDeltaX;
				D3DXQuaternionRotationAxis(&rotDeltaX, &right, move.y * PI / 2);
				D3DXQUATERNION rotDeltaY;
				D3DXQuaternionRotationAxis(&rotDeltaY, &up, move.x * PI / 2);

				D3DXQuaternionMultiply(&rot, &rot, &rotDeltaX);
				D3DXQuaternionMultiply(&rot, &rot, &rotDeltaY);
				//rot += rotDeltaX + rotDeltaY;
				//pos += (-right * move.x - up * move.y) * m_MoveSpeedMouse;
			}
		}

		m_Transform3D->SetPosition(pos);
		m_Transform3D->SetRotation(rot);
	}
}