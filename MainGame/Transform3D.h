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
	// @brief	このフレームに更新が必要だかの判断
	bool m_NeedUpdate;
public:
	void DisableUpdateThisFrame() { m_NeedUpdate = false; };

private:
	D3DXVECTOR3 m_Position;
	D3DXQUATERNION m_Rotation;
	D3DXVECTOR3 m_Scale;

	// @brief	親ノード
	Transform3D* m_Parent;

	// @brief	自分の変換行列
	// @details	描画に使ったり、子ノードの変換に影響したり
	D3DXMATRIX m_WorldMatrix;


	/*********************************************************
	* @brief	ローカル変換
	********************************************************/
public:
	// @brief	位置座標の設定と取得
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
	
	// @brief	クォータニオンの設定と取得
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

	// @brief	スケールの設定と取得
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
	* @brief	ワールド変換
	********************************************************/
public:
	// @brief	親ノードの設定と取得
	void SetParent(Transform3D* parent) { m_Parent = parent; };
	Transform3D* GetParent() { return m_Parent; };

	// @brief	自分の変換行列
	void SetWorldMatrix(const D3DXMATRIX& world) { m_WorldMatrix = world; };
	D3DXMATRIX GetWorldMatrix()const { return m_WorldMatrix; };

	// @brief	ワールド位置座標の取得
	D3DXVECTOR3 GetWorldPosition()const;

	// @brief	ワールドスケールの取得
	D3DXVECTOR3 GetWorldScale()const;

	// @brief	軸方向の取得
	D3DXVECTOR3 GetForward()const;
	D3DXVECTOR3 GetUp()const;
	D3DXVECTOR3 GetRight()const;


	// @brief	前方向（z軸正方向）を設定
	void SetForwardTo(const D3DXVECTOR3& targetPos);
	// @brief	対象ビュー空間で、視線の逆方向へ向かう
	void LookAtView(const D3DXMATRIX& view);
	// @brief	対象位置座標で、その対象を向ける
	void LookAt(const D3DXVECTOR3& targetPos);

};


