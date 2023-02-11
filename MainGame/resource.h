/*********************************************************
* @brief	�t�@�C�����d���Ƀ��[�h���Ȃ��悤�A
*			���\�[�X�𓯈�Ǘ�����B
********************************************************/
#pragma once
#include <map>
#include <string>
#include "model.h"


class TextureFactory
{
private:
	/*********************************************************
	* @brief	�摜����ĂɊǗ�����
	********************************************************/
	static std::map<std::string, ID3D11ShaderResourceView*> m_Textures;

public:
	static ID3D11ShaderResourceView* Load(std::string name);
	static void Clear();
};

class ModelFactory
{
private:
	/*********************************************************
	* @brief	���f��������ĂɊǗ�����
	********************************************************/
	static std::map<std::string, Model*> m_Models;

public:
	static Model* Load(std::string name);
	static void Clear();
};
