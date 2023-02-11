#pragma once
#include "DrawObject.h"
#include "Singleton.h"
#include <string>
#include <list>


/*********************************************************
* @brief	�l�p�`�̃|���S���A�C���X�^���X�`��p
********************************************************/
class Polygon2DMany :
	public DrawObject,
	public Singleton<Polygon2DMany>
{
private:
	enum BufferType
	{
		VERTEX,
		INSTANCE,
	};
	ID3D11Buffer* m_Buffer[2];//0:per vertex,1:per instance

	/*********************************************************
	* @brief	�C���X�^���X�f�[�^
	********************************************************/
private:
	int m_InstanceCount;
	INSTANCE_SQUARE m_InstanceDatas[INSTANCE_SQUARE_MAX];
public:	
	void AddInstanceData(const INSTANCE_SQUARE& data);
	void EndFrame();

public:
	void Draw()override;
	Polygon2DMany();
	~Polygon2DMany();
};



