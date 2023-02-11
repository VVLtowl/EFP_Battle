#include "main.h"
#include "TransformAnime.h"
#include "JBH_ShowRPSStart.h"

void JBH_ShowRPSStart::Start()
{
	//create game start UI object
	m_ShowUI.UIObject = GameObjectManager::Create<UIRPSStart>();

	//start ui in
	m_ShowUI.StartUIIn();
}

void JBH_ShowRPSStart::MainUpdate()
{
	if (m_ShowUI.State == ShowUIHelper::END)
	{
		NextState();
	}
	else if (m_ShowUI.State == ShowUIHelper::START_WAIT)
	{
		m_ShowUI.StartUIWait();
	}
	else if (m_ShowUI.State == ShowUIHelper::START_OUT)
	{
		m_ShowUI.StartUIOut();
	}
}

void JBH_ShowRPSStart::End()
{
	m_ShowUI.UIObject->SetState(GameObject::DEAD);
}
