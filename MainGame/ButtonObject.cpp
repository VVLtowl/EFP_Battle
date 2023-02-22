#include "main.h"
#include "Button.h"
#include "TransformAnime.h"
#include "LookAtCamera.h"
#include "ButtonAnimeHelper.h"
#include "RPS.h"
#include "Character.h"
#include "Piece.h"
#include "ButtonObject.h"

#include "Player.h"

#include "UIRPS.h"
#include "UIActSelection.h"
#include "UISquareSelection.h"

#pragma region ========== button object ==========
ButtonObject::ButtonObject(const D3DXVECTOR2& size) :
	m_LookAtCamera(nullptr),
	m_ImageSize(size)
{
	//create button
	{
		m_Button = new Button(this);
	}

	//create boxCollider
	{
		BoxColliderDescription boxDesc;
		boxDesc.AABBData.Min = {
			-m_ImageSize.x / 2.0f,
			-m_ImageSize.y / 2.0f,
			-0.1f
		};
		boxDesc.AABBData.Max = {
			m_ImageSize.x / 2.0f,
			m_ImageSize.y / 2.0f,
			0.1f
		};
		m_BoxCollider = new BoxCollider(this, boxDesc);
	}

	//create look at camera
	if (0)
	{
		m_LookAtCamera =
			new LookAtCamera(this, CameraManager::GetMainCamera());
	}
}

ButtonObject::~ButtonObject()
{
	if (m_ButtonHelper != nullptr)
	{
		delete m_ButtonHelper;
		m_ButtonHelper = nullptr;
	}
}

const Collision::AABB ButtonObject::GetAABB()const
{
	return m_BoxCollider->AABBData;
}

Button* ButtonObject::GetButton()
{
	return m_Button;
}

#pragma endregion

#pragma region ========== button hand ==========
ButtonHand::ButtonHand(Piece* piece):
	ButtonObject(UI_RPS_SIZE),
	Type(HandType::NONE),
	TargetPiece(piece)
{
	//create UI
	{
		m_UIRPS = new UIRPS();
		m_UIRPS->SetParent(this);
	}

	//create and set button anime helper
	{
		m_ButtonHelper =
			new ButtonHelper(m_UIRPS, GetButton(), m_UIRPS->GetPolygon2D());

		//init button anime
		m_ButtonHelper->InitButtonAnime();
	}

}

ButtonHand::~ButtonHand()
{
}

void ButtonHand::UpdateGameObject()
{
}

void ButtonHand::SetHandType(HandType type)
{
	m_UIRPS->SetRPS((int)type);
	Type = type;
}

void ButtonHand::SetPieceHand()
{
	TargetPiece->m_Hands.emplace_back(Type);
}
#pragma endregion

#pragma region ========== button act selection ==========
ButtonActSelection::ButtonActSelection(Piece* piece):
	ButtonObject(UI_ACTSELECTION_SIZE),
	m_TargetPiece(piece)
{
	//create UI
	{
		m_UIActSelection = new UIActSelection();
		m_UIActSelection->SetParent(this);
	}

	//create and set button anime helper
	{
		m_ButtonHelper =
			new ButtonHelper(m_UIActSelection, GetButton(), 
				m_UIActSelection->GetPolygon2D());

		//init button anime
		m_ButtonHelper->InitButtonAnime();
	}
}

ButtonActSelection::~ButtonActSelection()
{ 
}

void ButtonActSelection::UpdateGameObject()
{
}
#pragma endregion

#pragma region ========== button move ==========

ButtonInputMove::ButtonInputMove(Piece* piece, Square* square):
	ButtonObject(UI_ACTSELECTION_SIZE),
	m_TargetPiece(piece),
	m_TargetSquare(square)
{
}

#pragma region ========== button square selection ==========

ButtonSquareSelection::ButtonSquareSelection(Piece* piece, Square* square) :
	ButtonInputMove(piece,square)
{
	//create UI
	{
		m_UISquareSelection = new UISquareSelection();
		m_UISquareSelection->SetParent(this);
	}

	//create and set button anime helper
	{
		m_ButtonHelper =
			new ButtonHelper(m_UISquareSelection, GetButton(),
				m_UISquareSelection->GetPolygon2D());

		//init button anime
		m_ButtonHelper->InitButtonAnime();
	}
}

ButtonSquareSelection::~ButtonSquareSelection()
{
}

void ButtonSquareSelection::UpdateGameObject()
{
}
#pragma endregion

#pragma region ========== button cancel ==========

ButtonMoveCancel::ButtonMoveCancel(Piece* piece, Square* square) :
	ButtonInputMove(piece, square)
{
	//create UI
	{
		m_UICancel = new UIActSelection();
		m_UICancel->SetActTex(TEXID_UI_BACK);
		m_UICancel->SetParent(this);
	}

	//create look at m_LookAtCmr
	{
		//m_LookAtCmr = new LookAtCamera(this, CameraManager::GetMainCamera());
	}

	//create and set button anime helper
	{
		m_ButtonHelper =
			new ButtonHelper(m_UICancel, GetButton(),
				m_UICancel->GetPolygon2D());

		//set highlight color red before init anime
		m_ButtonHelper->m_ButtonAniDesc.HighlightUIColor = { 1,0,0,1 };

		//init button anime
		m_ButtonHelper->InitButtonAnime();
	}
}

ButtonMoveCancel::~ButtonMoveCancel()
{
}

void ButtonMoveCancel::UpdateGameObject()
{
}
#pragma endregion
#pragma endregion
