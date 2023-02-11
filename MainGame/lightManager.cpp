#include "main.h"
#include "lightManager.h"

MyLight* LightManager::m_MainShadowLight;

void LightManager::SetMainLight(MyLight* light)
{
	m_MainShadowLight = light;
}

MyLight* LightManager::GetMainLight()
{
	return m_MainShadowLight;
}

void LightManager::SelectMainLightView()
{
	//���C�g�J�����̍s����Z�b�g
	LIGHT light = m_MainShadowLight->LightData;
	Renderer::SetViewMatrix(&light.ViewMatrix);
	Renderer::SetProjectionMatrix(&light.ProjectionMatrix);
}