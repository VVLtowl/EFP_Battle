#include "main.h"
#include "UITurnStart.h"

UITurnStart::UITurnStart()
{
	//create polygonUI
	{
		PolygonUIDescription desc;
		desc.Size = UI_TURN_START_SIZE;
		desc.TextureID = TEXID_UI_TURN_START;
		desc.DrawObjDesc.Layer = LAYER_SCREEN_START_UI;
		m_TurnUI = new PolygonUI(this, desc);
	}

	//create NumbersUI
	{
		m_NumbersUI = new UINumbers(LAYER_SCREEN_START_UI);
		m_NumbersUI->SetParent(this);
		m_NumbersUI->GetTransform()->SetPosition(0, -1, 0);
		//m_NumbersUI->GetTransform()->SetParent(m_Transform3D);
	}
}

UITurnStart::~UITurnStart()
{
	//m_NumbersUI->GetTransform()->SetParent(nullptr);
	//m_NumbersUI->SetState(GameObject::DEAD);
}

void UITurnStart::UpdateGameObject()
{

}

void UITurnStart::SetNumber(int num)
{
	int digitsCount = 0;
	int tempNum = num;
	while (tempNum > 0)
	{
		tempNum /= 10;
		digitsCount++;
	}
	float offsetX = 
		UI_TURN_START_SIZE.x / 2.0f + digitsCount / 2.0f * UI_NUMBER_INTERVAL;

	m_NumbersUI->GetTransform()->SetPosition(offsetX, 0, 0);
	m_NumbersUI->SetNumber(num);
}
