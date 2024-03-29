#pragma once
#include <vector>
#include <typeinfo>
#include <list>
#include <string>


class GameObject
{
public:
	enum State
	{
		ACTIVE,
		PAUSED,
		DEAD
	};
protected:
	// @brief	自分所属のコンポネントを一斉に管理するため
	std::vector<class Component*> m_Components;
	std::list<Component*> m_PendingComponents;
	std::list<Component*> m_DeadComponents;
	bool m_NowUpdateComponent;

	// @brief	対象の状態
	State m_State;

	// @brief	空間的変換
	class Transform3D* m_Transform3D;

	// @brief	子オブジェクトのリスト、削除するとき子オブジェクトの親をnullptrに設定
	std::list<GameObject*> m_ChildObjects;
	GameObject* m_ParentObject;

	// @brief	名前
	std::string m_Name;
private:
	void UpdateComponents();
	virtual void UpdateGameObject() = 0;

public:
	/*********************************************************
	* @brief	生成する時、ゲーム対象をマネージャーに登録
	*			削除する時、ゲーム対象をマネージャーから除外
	********************************************************/
	GameObject();
	virtual ~GameObject();

	// @brief	ゲーム対象の更新
	void Update();

	/*********************************************************
	* @brief	継承先でコンポネントを追加し、
	*			ゲームオブジェクトを組み立てる
	********************************************************/
	// @brief	コンポネント追加と削除
	void AddComponent(class Component* cpn);
	void RemoveComponent(class Component* cpn);
	// @bried	具体のコンポネントを取得
	template<typename ComponentType>
	ComponentType* GetComponent();

	// @brief	状態の設定と取得
	void SetState(State state);
	State GetState() const { return m_State; }

	// @brief	名前の設定と取得
	void SetName(std::string name) { m_Name = name; }
	std::string GetName() const { return m_Name; }

	// @brief	空間的変換の取得
	Transform3D* GetTransform() { return m_Transform3D; };

	// @brief	親オブジェクト、子オブジェクト管理
	void SetParent(GameObject* parent);
	void RegisterChild(GameObject* child) { m_ChildObjects.emplace_back(child); };
	void QuitChild(GameObject* child) { m_ChildObjects.remove(child); };
	void SetPositionIncludeChilds(const D3DXVECTOR3& pos);
};


template<typename ComponentType>
inline ComponentType* GameObject::GetComponent()
{
	for (auto cpn : m_Components)
	{
		if (typeid(*cpn) == typeid(ComponentType))
		{
			return (ComponentType*)cpn;
		}
	}
}