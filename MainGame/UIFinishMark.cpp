#include "main.h"
#include "WorldToScreen.h"
#include "UIFinishMark.h"

const float X_NUM = 5.0f;
const float Y_NUM = 1.0f;
const float TEXCOORD_W = 1.0f / X_NUM;
const float TEXCOORD_H = 1.0f / Y_NUM;

UIFinishMark::UIFinishMark()
{
	//create UIMark
	{
		OwnUIMark = GameObjectManager::Create<UIMark>(); 
		OwnUIMark->GetTransform()->SetParent(m_Transform3D);
		OwnUIMark->SetMark(UIMark::MARK_FINISH);
	}

	//create ui follow worldObject
	{
		FollowWorldObject = new UIFollowWorldObject(this);
	}

	//create buttonAniHelper
	{

	}
}

UIFinishMark::~UIFinishMark()
{
	OwnUIMark->SetState(GameObject::DEAD);
}

void UIFinishMark::UpdateGameObject()
{
}
