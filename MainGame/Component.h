#pragma once


/*********************************************************
* @brief	各コンポネント実行順番
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
* @brief	コンポネント
********************************************************/
class Component
{
protected:
	// @brief	持ち主
	class GameObject* m_Owner;

	// @brief	更新順番（小さい方が早く更新を行う）
	int m_UpdateOrder;

public:
	/*********************************************************
	* @brief	生成する時、持ち主に登録
	*			削除する時、持ち主から除外
	********************************************************/
	Component(class GameObject* owner, int order = 100);
	virtual ~Component();

	// @brief	継承先で更新をカスタマイズ
	virtual void Update();

	// @brief	持ち主を取得
	class GameObject* GetGameObject() { return m_Owner; };

	// @brief	更新順番を取得
	int GetUpdateOrder()const { return m_UpdateOrder; };

	/*********************************************************
	* @brief	持ち主のコンポネント更新リストを影響しないように削除
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


