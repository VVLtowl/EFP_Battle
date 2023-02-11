#pragma once
#pragma region ========== renderer obj data structure ==========

/*********************************************************
* @brief	���_
********************************************************/
struct VERTEX_3D
{
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Normal;
	D3DXVECTOR4 Diffuse;
	D3DXVECTOR2 TexCoord;
};

/*********************************************************
* @brief	�l�p�̃C���X�^���X
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
* @brief	�}�e���A��
********************************************************/
struct MATERIAL
{
	// @brief	����
	D3DXCOLOR	Ambient;

	// @brief	�g�U����
	D3DXCOLOR	Diffuse;

	// @brief	���ʔ���
	D3DXCOLOR	Specular;

	D3DXCOLOR	Emission;
	float		Shininess;
	float		Dummy[3];
};


/*********************************************************
* @brief	����
********************************************************/
struct LIGHT
{
	BOOL		Enable = true;
	BOOL		Dummy[3] = { false };
	
	D3DXVECTOR4	Direction;
	
	// @brief	�����v�Z�悤
	D3DXVECTOR4 Position;
	D3DXVECTOR4 Angle;
	
	D3DXCOLOR	Diffuse;
	D3DXCOLOR	Ambient;


	// @brief	���C�g�J�����r���[
	D3DXMATRIX ViewMatrix;

	// @brief	���C�g�v���W�F�N�V����
	D3DXMATRIX ProjectionMatrix;
};

/*********************************************************
* @brief	�J����
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
* @brief	�����_���[
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

	// @brief	�I�ׂ�X�e�[�g
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
	static ID3D11RasterizerState* RSWireframe;        // ���X�^�[��ԁF���C���[�t���[��
	static ID3D11RasterizerState* RSNoCull;            // ���X�^�[��ԁF�o�b�N�J�[�����O���Ȃ�
	static ID3D11RasterizerState* RSCullClockWise;    // ���X�^�[��ԁF���v���J�[�����O

	static ID3D11SamplerState* SSLinear;            // �T���v���[��ԁF���`�t�B���^�[
	static ID3D11SamplerState* SSAnistropic;        // �T���v���[��ԁF�e���ڃt�B���^�[

	static ID3D11BlendState* BSDefault;
	static ID3D11BlendState* BSNoColorWrite;        // �u�����h��ԁF�ʂ̏������݂��Ȃ�
	static ID3D11BlendState* BSTransparent;        // �u�����h��ԁF�����u�����h
	static ID3D11BlendState* BSAlphaToCoverage;    //�@�u�����h��ԁFAlpha-To-Coverage

	static ID3D11DepthStencilState* DSSDefault;
	static ID3D11DepthStencilState* DSSMarkMirror;        // �[�x�^�X�e���V����ԁF�~���[�ʒu�̃X�e���V��
	static ID3D11DepthStencilState* DSSDrawReflection;    // �[�x�^�X�e���V����ԁF���ʔ��˂̑Ώۂ�`�悷�鎞
	static ID3D11DepthStencilState* DSSNoDoubleBlend;    // �[�x�^�X�e���V����ԁF���ڂ̃u�����h���Ȃ�
	static ID3D11DepthStencilState* DSSNoDepthTest;        // �[�x�^�X�e���V����ԁF�[�x�e�X�g���Ȃ�
	static ID3D11DepthStencilState* DSSNoDepthWrite;        // �[�x�^�X�e���V����ԁF�[�x�e�X�g���邪�A�������݂��Ȃ�

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