#include "main.h"
#include "JBH_ShowActStart.h"

void JBH_ShowActStart::Start()
{
	//create game start UI object
	{

	}
	m_UI = GameObjectManager::Create<UIActStart>();
	m_UI->GetPolygonUI()->Texture = DrawManager::Textures[TEXID_UI_ACT_START_BAD];
	m_ShowUI.UIObject = m_UI;

	//reset ui tex by isbad
	{
		if (m_IsBad)
		{
			m_UI->GetPolygonUI()->Texture = DrawManager::Textures[TEXID_UI_ACT_START_BAD];
			m_BHID = BHID_JUD_SHOW_BAD_ACT_START;
		}
		else
		{
			m_UI->GetPolygonUI()->Texture = DrawManager::Textures[TEXID_UI_ACT_START_GOOD];
			m_BHID = BHID_JUD_SHOW_GOOD_ACT_START;
		}
	}

	//start ui in
	m_ShowUI.StartUIIn();
}

void JBH_ShowActStart::MainUpdate()
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

void JBH_ShowActStart::End()
{
	m_ShowUI.UIObject->SetState(GameObject::DEAD);
	m_ShowUI.UIObject = nullptr;
	m_UI = nullptr;
}

void JBH_ShowActStart::SetIsBad(bool bad)
{
	m_IsBad = bad;
}




