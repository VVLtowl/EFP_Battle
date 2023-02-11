#include "main.h"
#include <iostream>
#include <assert.h>

#include "resource.h"

#pragma region ========== texture ==========
std::map<std::string, ID3D11ShaderResourceView*> TextureFactory::m_Textures;

ID3D11ShaderResourceView* TextureFactory::Load(std::string name)
{
	ID3D11ShaderResourceView* tex = nullptr;
	std::map<std::string, ID3D11ShaderResourceView*>::iterator it
		= m_Textures.find(name);

	//まだロードされていない場合
	if (it == m_Textures.end())
	{
		//テクスチャ読み込み
		D3DX11CreateShaderResourceViewFromFile(
			Renderer::GetDevice(),
			name.c_str(),
			NULL,
			NULL,
			&tex,
			NULL
		);
		assert(tex);

		m_Textures.emplace(name, tex);
	}
	//存在していたリソースの場合
	else
	{
		tex = (*it).second;
	}

	return tex;
}

void TextureFactory::Clear()
{
	ID3D11ShaderResourceView* tex = nullptr;
	for (auto pair : m_Textures)
	{
		tex = pair.second;
		tex->Release();
		//delete tex;
	}
	m_Textures.clear();
}
#pragma endregion

#pragma region ========== model ==========
std::map<std::string, Model*> ModelFactory::m_Models;

Model * ModelFactory::Load(std::string name)
{
	Model* model = nullptr;
	std::map<std::string, Model*>::iterator it = m_Models.find(name);

	//まだロードされていない場合
	if (it == m_Models.end())
	{
		model = new Model();
		model->Load(name.c_str());

		m_Models.emplace(name, model);
	}
	//存在していたリソースの場合
	else
	{
		model = (*it).second;
	}

	return model;
}

void ModelFactory::Clear()
{
	Model* m = nullptr;
	for (auto pair : m_Models)
	{
		m = pair.second;
		m->Unload();
		//delete m;
	}
	m_Models.clear();
}
#pragma endregion
