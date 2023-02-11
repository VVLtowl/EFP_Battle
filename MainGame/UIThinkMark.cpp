#include "main.h"
#include "WorldToScreen.h"
#include "UIThinkMark.h"


const float X_NUM = 5.0f;
const float Y_NUM = 1.0f;
const float TEXCOORD_W = 1.0f / X_NUM;
const float TEXCOORD_H = 1.0f / Y_NUM;

UIThinkMark::UIThinkMark()
{
	//create UIMark
	{
		OwnUIMark = GameObjectManager::Create<UIMark>();
		OwnUIMark->GetTransform()->SetParent(m_Transform3D);
		OwnUIMark->SetMark(UIMark::MARK_THINK);
	}

	//create ui follow worldObject
	{
		FollowWorldObject = new UIFollowWorldObject(this);
	}
}

UIThinkMark::~UIThinkMark()
{
	OwnUIMark->SetState(GameObject::DEAD);
}

void UIThinkMark::UpdateGameObject()
{
}
