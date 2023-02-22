#include "main.h"
#include "UINumbers.h"

UINumbers::UINumbers(LayerType layer):
	m_LayerType(layer)
{
}

UINumbers::~UINumbers()
{
	//for (auto ui : m_NumberUIs)
	//{
	//		ui->SetState(GameObject::DEAD);
	//}
	//m_NumberUIs.clear();
}

void UINumbers::UpdateGameObject()
{
}

void UINumbers::SetNumber(int num)
{
	int numTemp = num;
	int digitsNum = 0;
	std::vector<int> nums;

	//numを各桁で格納
	bool hasNum = true;
	while (hasNum)
	{
		digitsNum = numTemp % 10;
		//小さい桁から大きい桁までの順
		nums.emplace_back(digitsNum);

		numTemp /= 10;
		hasNum = (numTemp == 0 ? false : true);
	}

	//m_NumberUIsのオブジェクト数と新しいnumの桁数が等しいのかを確認
	//足りない場合、UIオブジェクトを追加
	int delta = nums.size() - m_NumberUIs.size();
	if (delta > 0)
	{
		for (int i = 0; i < delta; i++)
		{
			UINumber* ui = new UINumber(m_LayerType);
			ui->SetParent(this);
			m_NumberUIs.emplace_back(ui);
		}
	}
	else
	{
		for (int i = 0; i > delta; i--)
		{
			auto ui = m_NumberUIs.back();
			ui->SetState(GameObject::DEAD);
			m_NumberUIs.pop_back();
		}
	}

	//各UIオブジェクトで数字を設定
	//小さい桁から大きい桁までの順
	int digitsCount = nums.size();
	float localPosX = ((float)(digitsCount - 1) / 2.0f) * UI_NUMBER_INTERVAL;
	for (int i = 0; i < digitsCount; i++)
	{
		m_NumberUIs[i]->SetNumber(nums[i]);
		m_NumberUIs[i]->GetTransform()->SetPosition(
			localPosX - UI_NUMBER_INTERVAL * i, 0, 0);
	}
}
