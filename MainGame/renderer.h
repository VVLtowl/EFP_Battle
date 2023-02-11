#pragma once
#pragma region ========== renderer obj data structure ==========

/*********************************************************
* @brief	頂点
********************************************************/
struct VERTEX_3D
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR4 Diffuse;
	D3DXVECTOR2 TexCoord;
};

/*********************************************************
* @brief	四角のインスタンス
********************************************************/
struct INSTANCE_SQUARE
{
	//D3DXVECTOR3 Position;
	D3DXMATRIX World = MTX_NOTRANSFORM;
	D3DXVECTOR4 Color = V4_ONE;

	INSTANCE_SQUARE() {};
	INSTANCE_SQUARE(const D3DXMATRIX& world,const D3DXVECTOR4& color)
	{
		World = world;
		Color = color;
	}
};

/*********************************************************
* @brief	マテリアル
********************************************************/
struct MATERIAL
{
	// @brief	環境光
	D3DXCOLOR	Ambient;

	// @brief	拡散反射
	D3DXCOLOR	Diffuse;

	// @brief	鏡面反射
	D3DXCOLOR	Specular;

	D3DXCOLOR	Emission;
	float		Shininess;
	float		Dummy[3];
};


/*********************************************************
* @brief	光源
********************************************************/
struct LIGHT
{
	BOOL		Enable = true;
	BOOL		Dummy[3] = { false };
	
	D3DXVECTOR4	Direction;
	
	// @brief	視錐計算よう
	D3DXVECTOR4 Position;
	D3DXVECTOR4 Angle;
	
	D3DXCOLOR	Diffuse;
	D3DXCOLOR	Ambient;


	// @brief	ライトカメラビュー
	D3DXMATRIX ViewMatrix;

	// @brief	ライトプロジェクション
	D3DXMATRIX ProjectionMatrix;
};

/*********************************************************
* @brief	カメラ
********************************************************/
struct CAMERA
{
	D3DXVECTOR4	Position;
	D3DXVECTOR4	Direction;

	D3DXMATRIX ViewMatrix;
	D3DXMATRIX ProjectionMatrix;
};
#pragma endregion


/*********************************************************
* @brief	レンダラー
********************************************************/
class Renderer
{
public:
	static D3D_FEATURE_LEVEL       m_FeatureLevel;

	static ID3D11Device* m_Device;
	static ID3D11DeviceContext* m_DeviceContext;
	static IDXGISwapChain* m_SwapChain;
	static ID3D11RenderTargetView* m_RenderTargetView;
	static ID3D11DepthStencilView* m_DepthStencilView;

	static ID3D11Buffer* m_WorldBuffer;
	static ID3D11Buffer* m_ViewBuffer;
	static ID3D11Buffer* m_ProjectionBuffer;
	static ID3D11Buffer* m_MaterialBuffer;
	static ID3D11Buffer* m_LightBuffer;
	static ID3D11Buffer* m_CameraBuffer;
	static ID3D11Buffer* m_ParameterBuffer;

	static ID3D11DepthStencilView* m_MainDSV;
	static ID3D11Texture2D* m_MainTex;
	static ID3D11RenderTargetView* m_MainRT;
	static ID3D11ShaderResourceView* m_MainSRV;

	static ID3D11DepthStencilView* m_ShadowDepthStencilView;
	static ID3D11ShaderResourceView* m_ShadowDepthShaderResourceView;

	// @brief	選べるステート
private:
	//give up
	//static ID3D11BlendState* m_BlendState;
	//give up
	//static ID3D11BlendState* m_BlendStateATC;
	//give up
	//static ID3D11DepthStencilState* m_DepthStateEnable;
	//give up
	//static ID3D11DepthStencilState* m_DepthStateDisable;
public:
	static ID3D11RasterizerState* RSDefault;
	static ID3D11RasterizerState* RSWireframe;        // レスター状態：ワイヤーフレーム
	static ID3D11RasterizerState* RSNoCull;            // レスター状態：バックカーリングしない
	static ID3D11RasterizerState* RSCullClockWise;    // レスター状態：時計回りカーリング

	static ID3D11SamplerState* SSLinear;            // サンプラー状態：線形フィルター
	static ID3D11SamplerState* SSAnistropic;        // サンプラー状態：各項目フィルター

	static ID3D11BlendState* BSDefault;
	static ID3D11BlendState* BSNoColorWrite;        // ブレンド状態：彩の書き込みしない
	static ID3D11BlendState* BSTransparent;        // ブレンド状態：透明ブレンド
	static ID3D11BlendState* BSAlphaToCoverage;    //　ブレンド状態：Alpha-To-Coverage

	static ID3D11DepthStencilState* DSSDefault;
	static ID3D11DepthStencilState* DSSMarkMirror;        // 深度／ステンシル状態：ミラー位置のステンシル
	static ID3D11DepthStencilState* DSSDrawReflection;    // 深度／ステンシル状態：鏡面反射の対象を描画する時
	static ID3D11DepthStencilState* DSSNoDoubleBlend;    // 深度／ステンシル状態：二回目のブレンドしない
	static ID3D11DepthStencilState* DSSNoDepthTest;        // 深度／ステンシル状態：深度テストしない
	static ID3D11DepthStencilState* DSSNoDepthWrite;        // 深度／ステンシル状態：深度テストするが、書き込みしない

	static CAMERA WVP3DCameraData;
	static bool NowWVP3D;
public:
	static void Init();
	static void Uninit();
	static void BeginDepth();
	static void BeginTest();
	static void Begin();
	static void Begin(class Camera* cameraComp);
	static void Begin(class MyLight* lightComp);
	static void End(bool change);

	static void SetAlphaCoverageEnable(bool Enable);
	static void SetDepthEnable(bool Enable);
	static void SetWorldViewProjection2D();
	static void SwitchWVP2D3D(bool to3D);
	static void SetWorldMatrix(D3DXMATRIX* WorldMatrix);
	static void SetViewMatrix(D3DXMATRIX* ViewMatrix);
	static void SetProjectionMatrix(D3DXMATRIX* ProjectionMatrix);
	static void SetMaterial(MATERIAL Material);
	static void SetLight(LIGHT Light);
	static void SetCamera(const CAMERA& Camera);
	static void SetParameter(D3DXVECTOR4 Parameter);

	static ID3D11Device* GetDevice(void) { return m_Device; }
	static ID3D11DeviceContext* GetDeviceContext(void) { return m_DeviceContext; }
	static ID3D11ShaderResourceView* GetShadowDepthTexture() { return m_ShadowDepthShaderResourceView; };

	static void CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName, D3D11_INPUT_ELEMENT_DESC* LayoutDesc,UINT ElementNum);
	static void CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName);
};