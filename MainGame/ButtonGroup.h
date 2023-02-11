#pragma once
#include "UIGroupControl.h"

class ButtonGroup :
    public UIGroupControl
{
public:
	ButtonGroup(class GameObject* owner);
	~ButtonGroup();
	void Update()override;
	virtual void OtherUpdate() {};

	/*********************************************************
	* @brief	state event system
	********************************************************/
public:
	enum class State
	{
		WORKING,
		BUTTON_SELECTED,
		FINISH,
	};
	void ChangeState(State state);
	std::function<void()> OnFinish;
	std::function<void()> OnButtonSelected;
protected:
	State ButtonGroupState;

	/*********************************************************
	* @brief	moue input
	********************************************************/
protected:
	class DrawLine* m_MouseHintLine;

	/*********************************************************
	* @brief	manage button 
	********************************************************/
public:
	void AddButtonObject(class ButtonObject* obj);
	void SetUpButtonsParent();
	std::list<class ButtonObject*>* GetButtonObjs()
	{
		return &m_ButtonObjects;
	};
protected:
	std::list<class ButtonObject*> m_ButtonObjects;

};

