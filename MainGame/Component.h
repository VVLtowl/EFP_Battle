#pragma once


/*********************************************************
* @brief	�e�R���|�l���g���s����
********************************************************/
enum CompUpdateOrder
{
	//camera
	COMP_CAMERA,

	//light
	COMP_MYLIGHT,

	//UI
	COMP_BUTTON,

	//draw object
	COMP_POLYGON3D,
	COMP_POLYGON2D,
	COMP_POLYGONUI,
	COMP_DRAWLINE,

	//collider
	COMP_BOXCOLI,

	//move control
	COMP_MOVECTRL,

	//anime
	COMP_TRANSFORMANIME,

	//look at
	COMP_LOOKATTRANSFORM,
	COMP_LOOKATVIEW,

	//ui
	COMP_WORLDTOSCREEN,
	COMP_UIGROUPCTRL,

	//transform
	COMP_TRANSFORM3D,


	COMP_COUNT,
};




/*********************************************************
* @brief	�R���|�l���g
********************************************************/
class Component
{
protected:
	// @brief	������
	class GameObject* m_Owner;

	// @brief	�X�V���ԁi���������������X�V���s���j
	int m_UpdateOrder;

public:
	/*********************************************************
	* @brief	�������鎞�A������ɓo�^
	*			�폜���鎞�A�����傩�珜�O
	********************************************************/
	Component(class GameObject* owner, int order = 100);
	virtual ~Component();

	// @brief	�p����ōX�V���J�X�^�}�C�Y
	virtual void Update();

	// @brief	��������擾
	class GameObject* GetGameObject() { return m_Owner; };

	// @brief	�X�V���Ԃ��擾
	int GetUpdateOrder()const { return m_UpdateOrder; };

	/*********************************************************
	* @brief	������̃R���|�l���g�X�V���X�g���e�����Ȃ��悤�ɍ폜
	********************************************************/
public:
	enum State
	{
		ACTIVE,
		PAUSED,
		DEAD
	};
	int GetState() { return m_State; };
	void SetState(State state) { m_State = state; };
private:
	State m_State;
};


