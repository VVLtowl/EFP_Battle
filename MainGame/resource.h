/*********************************************************
* @brief	ファイルを重複にロードしないよう、
*			リソースを同一管理する。
********************************************************/
#pragma once
#include <map>
#include <string>
#include "model.h"


class TextureFactory
{
private:
	/*********************************************************
	* @brief	画像を一斉に管理する
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
	* @brief	モデル情報を一斉に管理する
	********************************************************/
	static std::map<std::string, Model*> m_Models;

public:
	static Model* Load(std::string name);
	static void Clear();
};
