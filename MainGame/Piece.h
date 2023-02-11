#pragma once
#include <list>
#include <functional>
#include "Executor.h"
#include "RPS.h"


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

	/*********************************************************
	* @brief	�R�}�̑���
	********************************************************/
public:
	//piece id
	int ID = -1;

	//�w�c
	CampType Camp = CampType::NONE;

	//Character
	CharacterType CharaType = CharacterType::NORMALBAD;
	class Character* OwnCharacter = nullptr;

	//�R�}�̎�
	class Player* OwnerPlayer = nullptr;
	

	//State
	//State State;


	//�g���W�����
	//bool IsOpenStatus = false;

	//������}�X
	class Square* FootOnSquare = nullptr;

	//Hand
	std::list<HandType> Hands;

	//ActPoint
	int ActPoint = 0;

	/*********************************************************
	* @brief	piece data
	********************************************************/
public:
	//create in game scene
	class PieceObject* m_PieceObject = nullptr;

	//panel for control button group
	class ButtonsPanel* m_ButtonsPanel = nullptr;



	/*********************************************************
	* @brief	�q���g�pUI�I�u�W�F�N�g�i��ԃt���O�j
	********************************************************/
public:
	class UIFinishMark* FinishMark = nullptr;//finish flag
	class UIThinkMark* ThinkMark = nullptr;
	class UIActMark* ActMark = nullptr;
	std::list<class UIHand*> UIShowHands;//for show hand result
	class UIActPoint* UIShowActpoint = nullptr;//for check act point
	class UISelect* SelectedMark = nullptr;//for selected or highlighted

	void SetUISelect(bool show);
	void SetUIThink(bool show);
	void SetUIFinish(bool show);
	void SetUIHands(bool show);
	void SetUIActpoint(bool show);

	/*********************************************************
	* @brief	�s��
	********************************************************/
public:
	void StartInputHand(std::function<void()> endEvent = []() {; });//for player iterate
	void AddHand(HandType handType);
	void FinishSetHand(HandType handType);//for network command
	void StartInputSelectAct();
	void StartInputMove();

	class PieceInputHand* BH_InputHand;
	class PieceInput* BH_Input;
};