#include "main.h"
#include "resource.h"
#include "GameObject.h"
#include "cameraManager.h"
#include "drawManager.h"
#include <string>

std::list<DrawObject*> DrawManager::m_DrawObjects[LAYER_MAX];
std::map<short, ID3D11ShaderResourceView*> DrawManager::Textures;
std::map<short, Model*> DrawManager::Models;

ID3D11Buffer* DrawManager::SquareVertexBuffer;
ID3D11Buffer* DrawManager::SquareInstanceBuffer;
ID3D11ShaderResourceView* DrawManager::StructuredBufferSRV;

ID3D11VertexShader* DrawManager::VertexShader[SHADER_MAX];
ID3D11PixelShader* DrawManager::PixelShader[SHADER_MAX];
ID3D11InputLayout* DrawManager::VertexLayout[SHADER_MAX];

Polygon2DMany* DrawManager::DrawPolygon2DMany;

void DrawManager::Load()
{
#pragma region ========== load texture ==========
	Textures.emplace(
		TEXID_NUMBER_0,
		TextureFactory::Load("asset\\texture\\number.dds"));
	Textures.emplace(
		TEXID_FIELD004,
		TextureFactory::Load("asset\\texture\\field004.jpg"));
	Textures.emplace(
		TEXID_HINT,
		TextureFactory::Load("asset\\texture\\hint.png"));	
	Textures.emplace(
		TEXID_HINT2,
		TextureFactory::Load("asset\\texture\\hint2.png"));
	Textures.emplace(
		TEXID_CIRCLE_0,
		TextureFactory::Load("asset\\texture\\halfAlpha.png"));
	Textures.emplace(
		TEXID_OIL_3,
		TextureFactory::Load("asset\\texture\\oil3.png"));


	Textures.emplace(
		TEXID_PIECE_TEST,
		TextureFactory::Load("asset\\texture\\player_animation.png"));
	Textures.emplace(
		TEXID_TEST_PIECE_NORMALBAD,
		TextureFactory::Load("asset\\texture\\test_piece_normalBad.png"));
	Textures.emplace(
		TEXID_TEST_PIECE_NORMALGOOD,
		TextureFactory::Load("asset\\texture\\test_piece_normalGood.png"));

	Textures.emplace(
		TEXID_UI_GAMEOVER_BADWIN,
		TextureFactory::Load("asset\\texture\\ui_gameOver_badWin.png"));
	Textures.emplace(
		TEXID_UI_GAMEOVER_GOODWIN,
		TextureFactory::Load("asset\\texture\\ui_gameOver_goodWin.png"));


	Textures.emplace(
		TEXID_UI_GAME_START,
		TextureFactory::Load("asset\\texture\\ui_gameStart.png"));
	Textures.emplace(
		TEXID_UI_TURN_START,
		TextureFactory::Load("asset\\texture\\ui_turn.png"));
	Textures.emplace(
		TEXID_UI_RPS_START,
		TextureFactory::Load("asset\\texture\\ui_RPSStart.png"));
	Textures.emplace(
		TEXID_UI_ACT_START,
		TextureFactory::Load("asset\\texture\\ui_actStart.png"));
	Textures.emplace(
		TEXID_UI_ACT_START_BAD,
		TextureFactory::Load("asset\\texture\\ui_badActStart.png"));
	Textures.emplace(
		TEXID_UI_ACT_START_GOOD,
		TextureFactory::Load("asset\\texture\\ui_goodActStart.png"));


	Textures.emplace(
		TEXID_UI_RPS,
		TextureFactory::Load("asset\\texture\\RPS.png"));
	Textures.emplace(
		TEXID_UI_MARK,
		TextureFactory::Load("asset\\texture\\mark.png"));

	Textures.emplace(
		TEXID_UI_ACT_MOVE,
		TextureFactory::Load("asset\\texture\\ui_actSelection_move.png"));
	Textures.emplace(
		TEXID_UI_ACT_FINISH,
		TextureFactory::Load("asset\\texture\\ui_actSelection_finish.png"));
	Textures.emplace(
		TEXID_UI_BACK,
		TextureFactory::Load("asset\\texture\\ui_back.png"));
	Textures.emplace(
		TEXID_UI_SQUARE,
		TextureFactory::Load("asset\\texture\\button_square.png"));


	Textures.emplace(
		TEXID_UI_SERVER,
		TextureFactory::Load("asset\\texture\\button_server.png"));
	Textures.emplace(
		TEXID_UI_CLIENT,
		TextureFactory::Load("asset\\texture\\button_client.png"));
#pragma endregion

#pragma region ========== load model ==========
	Models.emplace(
		MDLID_TORUS,
		ModelFactory::Load("asset\\model\\torus.obj"));

	Models.emplace(
		MDLID_CUBE,
		ModelFactory::Load("asset\\model\\cube.obj"));

	//Models.emplace(
	//	MDLID_MAN,
	//	ModelFactory::Load("asset\\model\\Akai_Idle.fbx"));

	Models.emplace(
		MDLID_SPHERE,
		ModelFactory::Load("asset\\model\\sky.obj"));


	Models.emplace(
		MDLID_SQUARE_NORMAL,
		ModelFactory::Load("asset\\model\\square_normal.obj"));
	Models.emplace(
		MDLID_SQUARE_GOAL,
		ModelFactory::Load("asset\\model\\square_goal.obj"));
	Models.emplace(
		MDLID_SQUARE_PRISON,
		ModelFactory::Load("asset\\model\\square_prison.obj"));
#pragma endregion
	
#pragma region ========== create buffer ==========
	//四角プリゴン
	{	
		//頂点バッファ生成	
		D3D11_BUFFER_DESC bd{};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * 4;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		VERTEX_3D vertex[4];
		{
			float x = 0;
			float y = 0;
			float w = 1;
			float h = 1;

			float width = 1;
			float height = 1;
			float leftTopX = -width / 2.0f;
			float leftTopY = height / 2.0f;
			float z = 0;

			vertex[0].Position = { leftTopX,leftTopY,z };
			vertex[0].Normal = { 0,0,-1 };
			vertex[0].Diffuse = V4_ONE;
			vertex[0].TexCoord = { x,y };

			vertex[1].Position = { leftTopX + width,leftTopY,z };
			vertex[1].Normal = { 0,0,-1 };
			vertex[1].Diffuse = V4_ONE;
			vertex[1].TexCoord = { x + w,y };

			vertex[2].Position = { leftTopX,leftTopY - height,z };
			vertex[2].Normal = { 0,0,-1 };
			vertex[2].Diffuse = V4_ONE;
			vertex[2].TexCoord = { x,y + h };

			vertex[3].Position = { leftTopX + width,leftTopY - height,z };
			vertex[3].Normal = { 0,0,-1 };
			vertex[3].Diffuse = V4_ONE;
			vertex[3].TexCoord = { x + w,y + h };
		}
		D3D11_SUBRESOURCE_DATA sd{};
		sd.pSysMem = vertex;
		Renderer::GetDevice()->CreateBuffer(&bd, &sd, &SquareVertexBuffer);

		//インスタンスバッファ生成
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(INSTANCE_SQUARE) * INSTANCE_SQUARE_MAX;
		bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
		bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bd.StructureByteStride = sizeof(INSTANCE_SQUARE);
		INSTANCE_SQUARE instance[INSTANCE_SQUARE_MAX];
		sd.pSysMem = instance;
		Renderer::GetDevice()->CreateBuffer(&bd, &sd, &SquareInstanceBuffer);

		//hlslでStructuredBuffer<Data>を使うため、SRV作成
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = INSTANCE_SQUARE_MAX;
		Renderer::GetDevice()->CreateShaderResourceView(
			SquareInstanceBuffer, &srvDesc, &StructuredBufferSRV);
	}

	//common buffer desc
	D3D11_INPUT_ELEMENT_DESC normalLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 10, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3D11_INPUT_ELEMENT_DESC particleLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 6, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 10, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "World", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "World", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 4 * 4, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "World", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 4 * 8, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "World", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 4 * 12, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "Color", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 4 * 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	//shader blinn phong light
	{
		//頂点シェーダー生成
		Renderer::CreateVertexShader(
			&VertexShader[SHADER_BLINN],
			&VertexLayout[SHADER_BLINN],
			"BlinnPhongLightingVS.cso", 
			normalLayoutDesc,
			4
		);

		//ピックセルシェーダー生成
		Renderer::CreatePixelShader(
			&PixelShader[SHADER_BLINN],
			"BlinnPhongLightingPS.cso"
		);
	}

	//shader shadow
	{
		//頂点シェーダー生成
		Renderer::CreateVertexShader(
			&VertexShader[SHADER_SHADOW],
			&VertexLayout[SHADER_SHADOW],
			"DepthShadowMappingVS.cso",
			normalLayoutDesc,
			4
		);

		//ピックセルシェーダー生成
		Renderer::CreatePixelShader(
			&PixelShader[SHADER_SHADOW],
			"DepthShadowMappingPS.cso"
		);
	}

	//shader unlit
	{
		//頂点シェーダー生成
		Renderer::CreateVertexShader(
			&VertexShader[SHADER_UNLIT],
			&VertexLayout[SHADER_UNLIT],
			"unlitTextureVS.cso",
			normalLayoutDesc,
			4
		);

		//ピックセルシェーダー生成
		Renderer::CreatePixelShader(
			&PixelShader[SHADER_UNLIT],
			"unlitTexturePS.cso"
		);

		//頂点シェーダー生成
		Renderer::CreateVertexShader(
			&VertexShader[SHADER_UNLIT_INSTANCE],
			&VertexLayout[SHADER_UNLIT_INSTANCE],
			"unlitTextureInstancedVS.cso",
			particleLayoutDesc,
			9
		);

		//ピックセルシェーダー生成
		Renderer::CreatePixelShader(
			&PixelShader[SHADER_UNLIT_INSTANCE],
			"unlitTexturePS.cso"
		);
	}

#pragma endregion

#pragma region ========== create draw instance object singleton ==========
	DrawPolygon2DMany = Polygon2DMany::Instance();
#pragma endregion
}

void DrawManager::Unload()
{
#pragma region ========== unload texture ==========
	//for(auto pair: Textures)
	//{
	//	pair.second->Release();
	//}
	Textures.clear();
#pragma endregion

#pragma region ========== unload model ========
	//for (auto pair : Models)
	//{
	//	pair.second->Unload();
	//}
	Models.clear();
#pragma endregion

#pragma region ========== release buffer ==========
	
	SquareVertexBuffer->Release();


	for (int i = 0; i < SHADER_MAX; i++)
	{
		VertexShader[i]->Release();
		PixelShader[i]->Release();
		VertexLayout[i]->Release();
	}

#pragma endregion

#pragma region ========== delete draw instance object singleton ==========
	delete DrawPolygon2DMany;
#pragma endregion
}


void DrawManager::Register(DrawObject* drawObj)
{
	//同じ対象は複数回登録しないようにする
	for (auto d : m_DrawObjects[drawObj->Layer])
	{
		if (d == drawObj)
		{
			return;
		}
	}

	m_DrawObjects[drawObj->Layer].emplace_back(drawObj);
}

void DrawManager::Quit(DrawObject* drawObj)
{
	m_DrawObjects[drawObj->Layer].remove(drawObj);
}

void DrawManager::DrawWorldObject()
{
	for (int layer = 0; layer < LAYER_WORLD_UI; layer++)
	{
		for (auto drawObj : m_DrawObjects[layer])
		{
			if (drawObj->GetState() == Component::ACTIVE)
			{
				drawObj->Draw();
			}
		}
	}
}


void DrawManager::DrawAll()
{
	//test 
	//z sort
#if 0
	m_DrawObjects[LAYER_WORLD_ALPHA].sort(
		[](DrawObject* l, DrawObject* g)
		{
			D3DXVECTOR3 posL = l->GetGameObject()->GetTransform()->GetPosition();
			D3DXVECTOR3 posG = g->GetGameObject()->GetTransform()->GetPosition();
			D3DXVECTOR3 posEye = CameraManager::GetMainCamera()->GetGameObject()->GetTransform()->GetPosition();

			D3DXVECTOR4 direction = CameraManager::GetMainCamera()->CameraData.Direction;

			D3DXVECTOR3 dirEye = { direction.x,direction.y,direction.z };
			D3DXVECTOR3 dirL = posL - posEye;
			D3DXVECTOR3 dirG = posG - posEye;

			float lz = D3DXVec3Dot(&dirEye, &dirL);
			float gz = D3DXVec3Dot(&dirEye, &dirG);
			return lz > gz;
		}
	);
#endif

	for (int layer = 0; layer < LAYER_MAX; layer++)
	{
		for (auto drawObj : m_DrawObjects[layer])
		{
			if (drawObj->GetState() == Component::ACTIVE)
			{
				drawObj->Draw();
			}
		}
	}
}

void DrawManager::DrawShadowDepth()
{
	//test 
	//z sort
#if 0
	m_DrawObjects[LAYER_WORLD_SHADOW_ALPHA].sort(
		[](DrawObject* l, DrawObject* g)
		{
			D3DXVECTOR3 posL = l->GetGameObject()->GetTransform()->GetPosition();
			D3DXVECTOR3 posG = g->GetGameObject()->GetTransform()->GetPosition();
			D3DXVECTOR3 posEye = CameraManager::GetMainCamera()->GetGameObject()->GetTransform()->GetPosition();
			
			D3DXVECTOR4 direction =	CameraManager::GetMainCamera()->CameraData.Direction;
			
			D3DXVECTOR3 dirEye = { direction.x,direction.y,direction.z };
			D3DXVECTOR3 dirL = posL - posEye;
			D3DXVECTOR3 dirG = posG - posEye;

			float lz = D3DXVec3Dot(&dirEye, &dirL);
			float gz = D3DXVec3Dot(&dirEye, &dirG);
			return lz > gz;
		}
	);
#endif


	for (auto drawObj : m_DrawObjects[LAYER_WORLD_SHADOW_NORMAL])
	{
		if (drawObj->GetState() == Component::ACTIVE)
		{
			drawObj->Draw();
		}
	}
	for (auto drawObj : m_DrawObjects[LAYER_WORLD_SHADOW_ALPHA])
	{
		if (drawObj->GetState() == Component::ACTIVE)
		{
			drawObj->Draw();
		}
	}
	for (auto drawObj : m_DrawObjects[LAYER_WORLD_SHADOW_PLAYER])
	{
		if (drawObj->GetState() == Component::ACTIVE)
		{
			drawObj->Draw();
		}
	}
}


