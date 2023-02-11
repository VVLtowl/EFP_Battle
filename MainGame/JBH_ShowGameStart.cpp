#include "main.h"
#include "TransformAnime.h"
#include "JBH_ShowGameStart.h"

void JBH_ShowGameStart::Start()
{
	//create game start UI object
	m_ShowUI.UIObject = GameObjectManager::Create<UIGameStart>();

	//start ui in
	m_ShowUI.StartUIIn();
}

void JBH_ShowGameStart::MainUpdate()
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

void JBH_ShowGameStart::End()
{
	m_ShowUI.UIObject->SetState(GameObject::DEAD);
	m_ShowUI.UIObject = nullptr;
}


