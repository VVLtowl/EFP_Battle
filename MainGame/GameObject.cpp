#include "main.h"
#include "Transform3D.h"
#include "Component.h"
#include "gameObjectManager.h"
#include "GameObject.h"


GameObject::GameObject() :
	m_State(ACTIVE),
	m_Name("NameLess"),
	m_NowUpdateComponent(false),
	m_ParentObject(nullptr)
{
	//init child list
	m_ChildObjects.clear();

	//create transform3D
	m_Transform3D = new Transform3D(this);

	//ゲーム対象をマネージャーに登録
	GameObjectManager::Register(this);
}

GameObject::~GameObject()
{
	//delete components
	while (!m_Components.empty())
	{
		delete m_Components.back();
	}

	//ゲーム対象をマネージャーから除外
	GameObjectManager::Quit(this);
}

void GameObject::Update()
{
	if (m_State == ACTIVE)
	{
		UpdateComponents();
		UpdateGameObject();
	}
}

void GameObject::UpdateComponents()
{
	m_NowUpdateComponent = true;
	for (auto cpn : m_Components)
	{
		if (cpn->GetState() == Component::ACTIVE)
		{
			cpn->Update();
		}
		else if (cpn->GetState()== Component::DEAD)
		{
			m_DeadComponents.emplace_back(cpn);
		}
	}
	m_NowUpdateComponent = false;

	//add pending component
	for (auto cpn : m_PendingComponents)
	{
		AddComponent(cpn);
	}
	m_PendingComponents.clear();

	//delete dead component
	//while (!m_DeadComponents.empty())
	//{
	//	auto cpn = m_DeadComponents.back();
	//	m_DeadComponents.remove(cpn);
	//	delete cpn;
	//}
	for (auto cpn : m_DeadComponents)
	{
		delete cpn;
	}
	m_DeadComponents.clear();
}

void GameObject::AddComponent(Component* addCpn)
{
	if (m_NowUpdateComponent)
	{
		m_PendingComponents.emplace_back(addCpn);
	}
	else
	{
		// Find the insertion point in the sorted vector
		// (The first element with a order higher than me)
		int addCpnOrder = addCpn->GetUpdateOrder();
		auto iter = m_Components.begin();
		while (iter != m_Components.end())
		{
			if (addCpnOrder < (*iter)->GetUpdateOrder())
			{
				break;
			}
			iter++;
		}

		// Inserts element before position of iterator
		m_Components.insert(iter, addCpn);
	}
}

void GameObject::RemoveComponent(Component* removeCpn)
{
	auto iter = std::find(
		m_Components.begin(),
		m_Components.end(), 
		removeCpn);
	if (iter != m_Components.end())
	{
		m_Components.erase(iter);
	}
}

void GameObject::SetState(GameObject::State state)
{
	m_State = state; 
	if (m_State == GameObject::DEAD)
	{
		//stop all components
		for (auto comp : m_Components)
		{
			comp->SetState(Component::PAUSED);
		}

		//quit from parent
		{
			if (m_ParentObject)
			{
				m_ParentObject->QuitChild(this);
				m_Transform3D->SetParent(nullptr);
			}
		}

		//delete childs
		{
			for (auto child : m_ChildObjects)
			{
				child->SetParent(nullptr);
				child->SetState(DEAD);
			}
			m_ChildObjects.clear();
		}
	}
}

void GameObject::SetParent(GameObject* parent)
{
	m_ParentObject = parent;
	if (parent != nullptr)
	{
		m_Transform3D->SetParent(parent->GetTransform());
		parent->RegisterChild(this);
	}
	else
	{
		m_Transform3D->SetParent(nullptr);
	}
}

void GameObject::SetPositionIncludeChilds(const D3DXVECTOR3& pos)
{
	m_Transform3D->SetPosition(pos);
	m_Transform3D->DisableUpdateThisFrame();
	m_Transform3D->UpdateTransform();//calculate parent's world matrix at first

	//update childs pos
	for (auto child : m_ChildObjects)
	{
		child->GetTransform()->UpdateTransform();
		child->GetTransform()->DisableUpdateThisFrame();
	}
}
