#pragma once
#include "Transform.h"

class Transform3D :
	public Transform
{
public:
	Transform3D(class GameObject* owner, int updateOrder = COMP_TRANSFORM3D);
	~Transform3D();
	void Update()override;
	void UpdateTransform();

private:
	// @brief	���̃t���[���ɍX�V���K�v�����̔��f
	bool m_NeedUpdate;
public:
	void DisableUpdateThisFrame() { m_NeedUpdate = false; };

private:
	D3DXVECTOR3 m_Position;
	D3DXQUATERNION m_Rotation;
	D3DXVECTOR3 m_Scale;

	// @brief	�e�m�[�h
	Transform3D* m_Parent;

	// @brief	�����̕ϊ��s��
	// @details	�`��Ɏg������A�q�m�[�h�̕ϊ��ɉe��������
	D3DXMATRIX m_WorldMatrix;


	/*********************************************************
	* @brief	���[�J���ϊ�
	********************************************************/
public:
	// @brief	�ʒu���W�̐ݒ�Ǝ擾
	void SetPosition(const D3DXVECTOR3& pos)
	{
		m_Position = pos;
	}
	void SetPosition(float x,float y,float z)
	{
		m_Position.x = x;
		m_Position.y = y;
		m_Position.z = z;
	}
	D3DXVECTOR3 GetPosition()const
	{
		return m_Position;
	}
	
	// @brief	�N�H�[�^�j�I���̐ݒ�Ǝ擾
	void SetRotation(const D3DXQUATERNION& rot)
	{
		m_Rotation = rot;
	}
	void SetRotation(float x,float y,float z)
	{
		D3DXQUATERNION quat = QUAT_IDENTITY;
		D3DXQuaternionRotationYawPitchRoll(&quat, y, x, z);
		SetRotation(quat);
	}
	D3DXQUATERNION GetRotation()const
	{
		return m_Rotation;
	}

	// @brief	�X�P�[���̐ݒ�Ǝ擾
	void SetScale(const D3DXVECTOR3& scl)
	{
		m_Scale = scl;
	}
	void SetScale(float x, float y, float z)
	{
		m_Scale.x = x;
		m_Scale.y = y;
		m_Scale.z = z;
	}
	D3DXVECTOR3 GetScale()const
	{
		return m_Scale;
	}
		

	/*********************************************************
	* @brief	���[���h�ϊ�
	********************************************************/
public:
	// @brief	�e�m�[�h�̐ݒ�Ǝ擾
	void SetParent(Transform3D* parent) { m_Parent = parent; };
	Transform3D* GetParent() { return m_Parent; };

	// @brief	�����̕ϊ��s��
	void SetWorldMatrix(const D3DXMATRIX& world) { m_WorldMatrix = world; };
	D3DXMATRIX GetWorldMatrix()const { return m_WorldMatrix; };

	// @brief	���[���h�ʒu���W�̎擾
	D3DXVECTOR3 GetWorldPosition()const;

	// @brief	���[���h�X�P�[���̎擾
	D3DXVECTOR3 GetWorldScale()const;

	// @brief	�������̎擾
	D3DXVECTOR3 GetForward()const;
	D3DXVECTOR3 GetUp()const;
	D3DXVECTOR3 GetRight()const;


	// @brief	�O�����iz���������j��ݒ�
	void SetForwardTo(const D3DXVECTOR3& targetPos);
	// @brief	�Ώۃr���[��ԂŁA�����̋t�����֌�����
	void LookAtView(const D3DXMATRIX& view);
	// @brief	�Ώۈʒu���W�ŁA���̑Ώۂ�������
	void LookAt(const D3DXVECTOR3& targetPos);

};


