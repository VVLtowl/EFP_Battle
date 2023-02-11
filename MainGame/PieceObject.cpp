#include "main.h"
#include "LookAtCamera.h"
#include "PieceObject.h"

PieceObject::PieceObject()
{
	//set name
	//SetName("PieceObject");

	//create ui transform
	{
		//root
		m_UITransform[(int)UITID::ROOT] = GameObjectManager::Create<TransformObject>();
		m_UITransform[(int)UITID::ROOT]->SetParent(this);
		m_UITransform[(int)UITID::ROOT]->InitLocalPosition(D3DXVECTOR3(0, 0, 0));

		//pawn ceneter
		m_UITransform[(int)UITID::PAWN_CENTER] = GameObjectManager::Create<TransformObject>();
		m_UITransform[(int)UITID::PAWN_CENTER]->SetParent(this);
		m_UITransform[(int)UITID::PAWN_CENTER]->InitLocalPosition(D3DXVECTOR3(0, PIECE_IMAGE_SIZE.y / 2, 0));

		//pawn up
		m_UITransform[(int)UITID::PAWN_UP] = GameObjectManager::Create<TransformObject>();
		m_UITransform[(int)UITID::PAWN_UP]->SetParent(this);
		m_UITransform[(int)UITID::PAWN_UP]->InitLocalPosition(D3DXVECTOR3(0, PIECE_IMAGE_SIZE.y, 0));
	
		//pawn up left
		m_UITransform[(int)UITID::PAWN_UP_LEFT] = GameObjectManager::Create<TransformObject>();
		m_UITransform[(int)UITID::PAWN_UP_LEFT]->SetParent(this);
		m_UITransform[(int)UITID::PAWN_UP_LEFT]->InitLocalPosition(D3DXVECTOR3(-PIECE_IMAGE_SIZE.x / 3.0f, PIECE_IMAGE_SIZE.y * 3 / 4, 0));
		
		//pawn up right
		m_UITransform[(int)UITID::PAWN_UP_RIGHT] = GameObjectManager::Create<TransformObject>();
		m_UITransform[(int)UITID::PAWN_UP_RIGHT]->SetParent(this);
		m_UITransform[(int)UITID::PAWN_UP_RIGHT]->InitLocalPosition(D3DXVECTOR3(PIECE_IMAGE_SIZE.x / 3.0f, PIECE_IMAGE_SIZE.y * 3 / 4, 0));
	}

	//create piece
	{
		m_Pawn = GameObjectManager::Create<Pawn_Normal>();
		m_Pawn->GetTransform()->SetPosition(0, PIECE_IMAGE_SIZE.y / 2.0f, 0);
		m_Pawn->SetParent(GetUITransform(UITID::ROOT));
	}

	//create look at camera
	{
		m_LookAtCamera =
			new LookAtCamera(this, CameraManager::GetMainCamera());
	}
}

PieceObject::~PieceObject()
{
	//m_Pawn->SetState(GameObject::DEAD);
	//for (int i = 0; i < UITID::MAX; i++)
	//{
	//	m_UITransform[i]->SetState(GameObject::DEAD);
	//}
}

void PieceObject::UpdateGameObject()
{
}

void PieceObject::SetTexColor(const D3DXVECTOR4& cl)
{
	m_Pawn->PieceImage->Color = cl;
}

GameObject* PieceObject::GetUITransform(UITID uitID)
{
	return m_UITransform[(int)uitID]; 
}
