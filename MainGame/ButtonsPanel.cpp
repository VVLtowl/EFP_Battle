#include "main.h"
#include "ButtonsPanel.h"

#include "LookAtCamera.h"
#include "MyButtonGroup.h"


ButtonsPanel::ButtonsPanel()
{
	//create ButtonHandGroup
	{
		m_ButtonGroup = new ButtonGroup(this);
	}

	//create LookAtCmr
	{
		Camera* cmr = CameraManager::GetMainCamera();
		m_LookAtCmr = new LookAtCamera(this, cmr);
	}
}

ButtonsPanel::~ButtonsPanel()
{
}

void ButtonsPanel::UpdateGameObject()
{
}
