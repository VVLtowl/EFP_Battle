#pragma once

/*********************************************************
* @brief	四角インスタンスの最大数
********************************************************/
const size_t INSTANCE_SQUARE_MAX=10000;

/*********************************************************
* @brief	テキスチャーID
********************************************************/
enum TextureID
{
	TEXID_NUMBER_0,
	TEXID_FIELD004,
	TEXID_HINT,
	TEXID_CIRCLE_0,

	TEXID_PIECE_TEST,

	TEXID_UI_GAME_START,
	TEXID_UI_TURN_START,
	TEXID_UI_RPS_START,
	TEXID_UI_ACT_START_BAD,
	TEXID_UI_ACT_START_GOOD,
	
	TEXID_UI_RPS,
	TEXID_UI_MARK,

	TEXID_MAX,
};

/*********************************************************
* @brief	モデルID
********************************************************/
enum ModelID
{
	MDLID_TORUS,
	MDLID_CUBE,
	MDLID_MAN,
	MDLID_SPHERE,

	MDLID_MAX,
};

/*********************************************************
* @brief	シェーダータイプ
********************************************************/
enum ShaderType
{
	SHADER_BLINN,
	SHADER_SHADOW,
	SHADER_UNLIT,
	SHADER_UNLIT_INSTANCE,

	SHADER_MAX,
};

#include "DrawObject.h"
#include <list>
#include <map>

/*********************************************************
* @brief	描画対象　ヘッダー
********************************************************/
#include "Polygon2D.h"
#include "Polygon2DMany.h"
#include "Polygon3D.h"
#include "PolygonUI.h"
#include "DrawLine.h"



class DrawManager
{
	/*********************************************************
	* @brief	描画対象を一斉に管理する
	********************************************************/
private:
	static std::list<DrawObject*> m_DrawObjects[];
public:
	static std::map<short, ID3D11ShaderResourceView*> Textures;
	static std::map<short, Model*> Models;
	static ID3D11Buffer* SquareVertexBuffer;
	static ID3D11Buffer* SquareInstanceBuffer;
	static ID3D11ShaderResourceView* StructuredBufferSRV;

	static ID3D11VertexShader* VertexShader[];
	static ID3D11PixelShader* PixelShader[];
	static ID3D11InputLayout* VertexLayout[];

	static Polygon2DMany* DrawPolygon2DMany;
public:
	static void Load();
	static void Unload();

	static void DrawAll();
	static void DrawWorldObject();
	static void DrawShadowDepth();

	static void Register(DrawObject* drawObj);
	static void Quit(DrawObject* drawObj);
};
