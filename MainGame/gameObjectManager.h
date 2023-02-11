#pragma once
#include "GameObject.h"
#include <list>

/*********************************************************
* @brief	�Q�[���I�u�W�F�N�g�@�w�b�_�[
********************************************************/
//test
#include "LineCollisionTester.h"
#include "DepthUI.h"
#include "Emitter.h"
#include "EffectParticle.h"

//standard
#include "TransformObject.h"
#include "MainCamera.h"
#include "FreeCamera.h"
#include "MainLight.h"
#include "Torus.h"
#include "Plane.h"
#include "Cube.h"
#include "Sphere.h"
#include "UIPanel.h"
#include "ActSelection.h"
#include "ParticleSystem.h"
#include "UINumber.h"
#include "UINumbers.h"
#include "LinePainter.h"//singleton
#include "MousePointRay.h"//singleton

//prefab
#include "PieceObject.h"
#include "Pawn_Normal.h"
#include "SquareObject.h"
#include "UIGameStart.h"
#include "UITurnStart.h"
#include "UIRPSStart.h"
#include "UIActStart.h"
#include "UIRPS.h"
#include "UIMark.h"
#include "UISelect.h"
#include "UIFinishMark.h"
#include "UIActMark.h"
#include "UIActPoint.h"

class GameObjectManager
{
private:
	// @brief	�Q�[���Ώۂ���ĂɊǗ����郊�X�g
	static std::list<GameObject*> m_GameObjects;
	// @brief	�Ǘ����X�g�֒ǉ���҂��Ă�Ώۂ��ꎞ�ۗ��p���X�g
	static std::list<GameObject*> m_PendingGameObjects;

	// @brief	�ΏۍX�V���ł��邩
	// @details	�ΏۍX�V���Ă���ԁA���X�g�v�f�̑}���ƍ폜���s��Ȃ�
	static bool m_NowUpdateGameObject;

public:
	template <typename GameObjectType>
	static GameObjectType* Create();

	static void InitShowObjsInfo();
	static void Update();
	static void Clear();

	static void Register(GameObject* gameObj);
	static void Quit(GameObject* gameObj);
};

template<typename GameObjectType>
inline GameObjectType* GameObjectManager::Create()
{
	GameObjectType* obj = new GameObjectType();
	return obj;
}




