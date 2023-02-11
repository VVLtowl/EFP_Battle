#include "main.h"
#include "cameraManager.h"

Camera* CameraManager::m_MainCamera;
std::list<Camera*> CameraManager::m_Cameras;

void CameraManager::SetMainCamera(Camera* cmr)
{
	m_MainCamera = cmr;
}

Camera* CameraManager::GetMainCamera()
{
	return m_MainCamera;
}

void CameraManager::SelectMainCameraView()
{
	CAMERA camera = m_MainCamera->CameraData;
	Renderer::SetViewMatrix(&camera.ViewMatrix);
	Renderer::SetProjectionMatrix(&camera.ProjectionMatrix);
}

void CameraManager::Register(Camera* cmr)
{
	m_Cameras.emplace_back(cmr);
}

void CameraManager::Quit(Camera* cmr)
{
	m_Cameras.remove(cmr);
}

void CameraManager::Init()
{
	m_MainCamera = nullptr;
	m_Cameras.clear();
}

void CameraManager::UpdateCameraDraw()
{
	for (auto cmr : m_Cameras)
	{
		Renderer::SetCamera(cmr->CameraData);
		cmr->Draw();
	}
}
