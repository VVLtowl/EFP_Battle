#include "main.h"
#include "TransformAnime.h"
#include "JBH_ShowTurnStart.h"

JBH_ShowTurnStart::JBH_ShowTurnStart()
{
	//create game start UI object
	m_TurnStart = GameObjectManager::Create<UITurnStart>();
	m_ShowUI.UIObject = m_TurnStart;

	//modify anime position
	m_ShowUI.ShowUIDesc.PositionX = SCREEN_WIDTH / 2 - UI_TURN_START_SIZE.x / 3.0f;
	m_TurnStart->GetTransform()->SetPosition(m_ShowUI.ShowUIDesc.PositionUIInStart());
}

JBH_ShowTurnStart::~JBH_ShowTurnStart()
{
	m_TurnStart->SetState(GameObject::DEAD);
}

void JBH_ShowTurnStart::Start()
{
	//start ui in
	m_ShowUI.StartUIIn();

	//set ui number
	m_TurnStart->SetNumber(m_Judgement->TurnCount);
}

void JBH_ShowTurnStart::MainUpdate()
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