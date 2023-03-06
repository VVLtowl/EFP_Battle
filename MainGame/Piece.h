#pragma once
#include <list>
#include <functional>
#include "Executor.h"
#include "RPS.h"


const D3DXVECTOR3 OFFSET_PIECE_ON_SQUARE = D3DXVECTOR3(0, 0.5f, 0);

enum PieceType
{
	PIECE_NORMAL,
};


enum class CampType
{
	NONE,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
	BAD,
	GOOD,
};
std::string CampTypeName(CampType type);


struct PieceDesc
{
	CharacterType CharaType = CharacterType::NORMALBAD;
	CampType PieceCamp = CampType::BAD;
	int PieceID = -1;
	int PlayerID = -1;
};



class Piece:
	public Executor
{
public:
	Piece();
	~Piece();
	std::string Name()override { return "piece"+std::to_string(m_ID); };

	/*********************************************************
	* @brief	�R�}�̑���
	********************************************************/
public:
	//piece id
	int m_ID = -1;

	//�w�c
	CampType m_Camp = CampType::NONE;

	//Character
	CharacterType m_CharaType = CharacterType::NORMALBAD;
	class Character* m_OwnCharacter = nullptr;

	//�R�}�̎�
	class Player* m_OwnerPlayer = nullptr;
	

	//State
	enum class State
	{
		NORMAL,
		CAUGHT,
		ESCAPE,
	};
	State m_State = State::NORMAL;
	//State State;


	//�g���W�����
	//bool IsOpenStatus = false;

	//������}�X
	class Square* m_FootOnSquare = nullptr;

	//Hand
	std::list<HandType> m_Hands;

	//ActPoint
	int m_ActPoint = 0;

	/*********************************************************
	* @brief	piece data
	********************************************************/
public:
	//create in game scene
	class PieceObject* m_PieceObject = nullptr;

	//panel for control button group
	class ButtonsPanel* m_ButtonsPanel = nullptr;

	//temp for next target square
	class Square* m_MoveToSquare;

	/*********************************************************
	* @brief	�q���g�pUI�I�u�W�F�N�g�i��ԃt���O�j
	********************************************************/
public:
	class UIFinishMark* m_FinishMark = nullptr;//finish flag
	class UIThinkMark* m_ThinkMark = nullptr;
	std::list<class UIHand*> m_UIShowHands;//for show hand result
	class UIActPoint* m_UIShowActpoint = nullptr;//for check act point
	class UISelect* m_SelectedMark = nullptr;//for selected or highlighted

	void SetUISelect(bool show);
	void SetUIThink(bool show);
	void SetUIFinish(bool show);
	void SetUIHands(bool show);
	void SetUIActpoint(bool show);

	/*********************************************************
	* @brief	�s��
	********************************************************/
public:
	void AddHand(HandType handType);
	void FinishSetHand(HandType handType);//for network command
	void StartCameraLookAtThis(float duration = 45, std::function<void()> endEvent = []() {; });//reset in client

	void StartInputHand(std::function<void()> endEvent = []() {; });//for player iterate
	void StartInputSelectAct();
	void StartInputMove();

	void StartMove(int squareID);
	void StartCatch() {};//todo pair with StartCaught()
	void StartCaught(int prisonSquareID);
	void StartEscape(int escapeSquareID);

	class PieceInputHand* BH_InputHand;
	class PieceInput* BH_Input;
	class PieceCalculateCanGoSquare* BH_CalCanGoSquare;
};