#include "main.h"
#include <io.h>

#pragma region ========== renderer ==========
/*********************************************************
* @brief	スタティック変数定義
********************************************************/
D3D_FEATURE_LEVEL       Renderer::m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

ID3D11Device* Renderer::m_Device = nullptr;
ID3D11DeviceContext* Renderer::m_DeviceContext = nullptr;
IDXGISwapChain* Renderer::m_SwapChain = nullptr;

ID3D11RenderTargetView* Renderer::m_RenderTargetView = nullptr;
ID3D11DepthStencilView* Renderer::m_DepthStencilView = nullptr;
ID3D11DepthStencilView* Renderer::m_ShadowDepthStencilView = nullptr;
ID3D11ShaderResourceView* Renderer::m_ShadowDepthShaderResourceView = nullptr;

ID3D11DepthStencilView* Renderer::m_MainDSV = nullptr;
ID3D11Texture2D* Renderer::m_MainTex=nullptr;
ID3D11RenderTargetView* Renderer::m_MainRT=nullptr;
ID3D11ShaderResourceView* Renderer::m_MainSRV = nullptr;

ID3D11Buffer* Renderer::m_WorldBuffer = nullptr;
ID3D11Buffer* Renderer::m_ViewBuffer = nullptr;
ID3D11Buffer* Renderer::m_ProjectionBuffer = nullptr;
ID3D11Buffer* Renderer::m_MaterialBuffer = nullptr;
ID3D11Buffer* Renderer::m_LightBuffer = nullptr;
ID3D11Buffer* Renderer::m_CameraBuffer = nullptr;
ID3D11Buffer* Renderer::m_ParameterBuffer = nullptr;


// @brief	選べるステート
ID3D11RasterizerState* Renderer::RSDefault = nullptr;
ID3D11RasterizerState* Renderer::RSWireframe = nullptr;
ID3D11RasterizerState* Renderer::RSNoCull = nullptr;
ID3D11RasterizerState* Renderer::RSCullClockWise = nullptr;

ID3D11SamplerState* Renderer::SSLinear = nullptr;          
ID3D11SamplerState* Renderer::SSAnistropic = nullptr;      

ID3D11BlendState* Renderer::BSDefault = nullptr;
ID3D11BlendState* Renderer::BSNoColorWrite = nullptr;      
ID3D11BlendState* Renderer::BSTransparent = nullptr;       
ID3D11BlendState* Renderer::BSAlphaToCoverage = nullptr;   

ID3D11DepthStencilState* Renderer::DSSDefault = nullptr;
ID3D11DepthStencilState* Renderer::DSSMarkMirror = nullptr;
ID3D11DepthStencilState* Renderer::DSSDrawReflection = nullptr;
ID3D11DepthStencilState* Renderer::DSSNoDoubleBlend = nullptr;
ID3D11DepthStencilState* Renderer::DSSNoDepthTest = nullptr;
ID3D11DepthStencilState* Renderer::DSSNoDepthWrite = nullptr;

CAMERA Renderer:: WVP3DCameraData;
bool Renderer::NowWVP3D=true;

void Renderer::Init()
{
	HRESULT hr = S_OK;

	// デバイス、スワップチェーン作成
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = SCREEN_WIDTH;
	swapChainDesc.BufferDesc.Height = SCREEN_HEIGHT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = GetWindow();
	swapChainDesc.SampleDesc.Count = 8;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = TRUE;

	hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		NULL,
		0,
		D3D11_SDK_VERSION,

		&swapChainDesc,
		&m_SwapChain,
		&m_Device,
		&m_FeatureLevel,
		&m_DeviceContext);

	if (FAILED(hr))
	{
		return;
	}

	// レンダーターゲットビュー作成
	ID3D11Texture2D* renderTarget;
	m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&renderTarget));
	m_Device->CreateRenderTargetView(renderTarget, NULL, &m_RenderTargetView);
	renderTarget->Release();



	// デプスステンシルバッファ作成
	ID3D11Texture2D* depthStencileTex = NULL;
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = swapChainDesc.BufferDesc.Width;
	textureDesc.Height = swapChainDesc.BufferDesc.Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	//textureDesc.Format = DXGI_FORMAT_D16_UNORM;
	textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	textureDesc.SampleDesc = swapChainDesc.SampleDesc;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	hr = m_Device->CreateTexture2D(
		&textureDesc, 
		NULL,
		&depthStencileTex
	);


	// デプスステンシルビュー作成
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = textureDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;// D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = 0;
	hr = m_Device->CreateDepthStencilView(
		depthStencileTex,
		&depthStencilViewDesc, 
		&m_DepthStencilView
	);


	depthStencileTex->Release();

	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);


	// ビューポート設定
	D3D11_VIEWPORT viewport;
	viewport.Width = (FLOAT)SCREEN_WIDTH;
	viewport.Height = (FLOAT)SCREEN_HEIGHT;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	m_DeviceContext->RSSetViewports(1, &viewport);


	// ラスタライザステート設定
	{
		D3D11_RASTERIZER_DESC rasterizerDesc{};

		// レスター状態：デフォルト
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;//test
		rasterizerDesc.DepthClipEnable = TRUE;
		rasterizerDesc.MultisampleEnable = FALSE;
		m_Device->CreateRasterizerState(&rasterizerDesc, &RSDefault);

		// レスター状態：ワイヤーフレーム
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.MultisampleEnable = false;
		m_Device->CreateRasterizerState(&rasterizerDesc, &RSWireframe);

		// レスター状態：バックカーリングしない
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;//test
		rasterizerDesc.DepthClipEnable = TRUE;
		rasterizerDesc.MultisampleEnable = FALSE;
		m_Device->CreateRasterizerState(&rasterizerDesc, &RSNoCull);

		// レスター状態：時計回りカーリング
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise = true;
		rasterizerDesc.DepthClipEnable = true;
		m_Device->CreateRasterizerState(&rasterizerDesc, &RSCullClockWise);

	}
	m_DeviceContext->RSSetState(RSDefault);


	// サンプラーステート設定
	{
		D3D11_SAMPLER_DESC samplerDesc{};

		// サンプラー状態：線形フィルター
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		m_Device->CreateSamplerState(&samplerDesc, &SSLinear);

		// サンプラー状態：各項目フィルター
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;  
		samplerDesc.MaxAnisotropy = 4;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		m_Device->CreateSamplerState(&samplerDesc, &SSAnistropic);

	}
	m_DeviceContext->PSSetSamplers(0, 1, &SSLinear);

	// ブレンドステート設定
	{
		D3D11_BLEND_DESC blendDesc{};

		// ブレンド状態：デフォルト
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = false;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		m_Device->CreateBlendState(&blendDesc, &BSDefault);

		//　ブレンド状態：Alpha-To-Coverage
		blendDesc.AlphaToCoverageEnable = true;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = false;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		m_Device->CreateBlendState(&blendDesc, &BSAlphaToCoverage);

		// ブレンド状態：彩の書き込みしない
		// Color = DestColor
		// Alpha = DestAlpha
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = false;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALPHA;
		m_Device->CreateBlendState(&blendDesc, &BSNoColorWrite);

		// ブレンド状態：透明ブレンド
		// Color = SrcAlpha * SrcColor + (1 - SrcAlpha) * DestColor 
		// Alpha = SrcAlpha
		// need zView order
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		m_Device->CreateBlendState(&blendDesc, &BSTransparent);

	}
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_DeviceContext->OMSetBlendState(BSAlphaToCoverage, blendFactor, 0xffffffff);

	// デプスステンシルステート設定
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};

		// 深度／ステンシル状態：デフォルト
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.StencilEnable = FALSE;
		m_Device->CreateDepthStencilState(&depthStencilDesc, &DSSDefault);//深度有効ステート

		// 深度／ステンシル状態：深度テストしない
		depthStencilDesc.DepthEnable = FALSE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.StencilEnable = FALSE;
		m_Device->CreateDepthStencilState(&depthStencilDesc, &DSSNoDepthTest);

		// 深度／ステンシル状態：深度テストするが、書き込みしない
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.StencilEnable = false;
		m_Device->CreateDepthStencilState(&depthStencilDesc, &DSSNoDepthWrite);

		// 深度／ステンシル状態：ミラー位置のステンシル
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		m_Device->CreateDepthStencilState(&depthStencilDesc, &DSSMarkMirror);

		// 深度／ステンシル状態：鏡面反射の対象を描画する時
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
		m_Device->CreateDepthStencilState(&depthStencilDesc, &DSSDrawReflection);

		// 深度／ステンシル状態：二回目のブレンドしない
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
		m_Device->CreateDepthStencilState(&depthStencilDesc, &DSSNoDoubleBlend);
	}
	m_DeviceContext->OMSetDepthStencilState(DSSDefault, NULL);

	// 定数バッファ生成
	{
		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.ByteWidth = sizeof(D3DXMATRIX);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = sizeof(float);

		m_Device->CreateBuffer(&bufferDesc, NULL, &m_WorldBuffer);
		m_DeviceContext->VSSetConstantBuffers(0, 1, &m_WorldBuffer);

		m_Device->CreateBuffer(&bufferDesc, NULL, &m_ViewBuffer);
		m_DeviceContext->VSSetConstantBuffers(1, 1, &m_ViewBuffer);

		m_Device->CreateBuffer(&bufferDesc, NULL, &m_ProjectionBuffer);
		m_DeviceContext->VSSetConstantBuffers(2, 1, &m_ProjectionBuffer);


		bufferDesc.ByteWidth = sizeof(MATERIAL);
		m_Device->CreateBuffer(&bufferDesc, NULL, &m_MaterialBuffer);
		m_DeviceContext->VSSetConstantBuffers(3, 1, &m_MaterialBuffer);


		bufferDesc.ByteWidth = sizeof(LIGHT);
		m_Device->CreateBuffer(&bufferDesc, NULL, &m_LightBuffer);
		m_DeviceContext->VSSetConstantBuffers(4, 1, &m_LightBuffer);
		m_DeviceContext->PSSetConstantBuffers(4, 1, &m_LightBuffer);

		bufferDesc.ByteWidth = sizeof(CAMERA);
		m_Device->CreateBuffer(&bufferDesc, NULL, &m_CameraBuffer);
		m_DeviceContext->VSSetConstantBuffers(5, 1, &m_CameraBuffer);
		m_DeviceContext->PSSetConstantBuffers(5, 1, &m_CameraBuffer);


		bufferDesc.ByteWidth = sizeof(D3DXVECTOR4);
		m_Device->CreateBuffer(&bufferDesc, NULL, &m_ParameterBuffer);
		m_DeviceContext->PSSetConstantBuffers(6, 1, &m_ParameterBuffer);
	}
	

	// マテリアル初期化
	{
		MATERIAL material{};
		material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		SetMaterial(material);
	}


	//深度描画準備
	{
		//シャドウバッファ作成
		ID3D11Texture2D* depthTexture = NULL;
		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(td));
		td.Width = swapChainDesc.BufferDesc.Width; //バックバッファのサイズを受けつぐ
		td.Height = swapChainDesc.BufferDesc.Height;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R32_TYPELESS;//32bitの自由な形式のデータとする
		//td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		td.SampleDesc = swapChainDesc.SampleDesc;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE; // デプス＆ステンシルバッファとして作成
		//td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;
		m_Device->CreateTexture2D(&td, NULL, &depthTexture);

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(dsvd));
		dsvd.Format = DXGI_FORMAT_D32_FLOAT;//ピクセルフォーマットは32BitFLOAT型
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dsvd.Flags = 0;
		m_Device->CreateDepthStencilView(
			depthTexture,
			&dsvd,
			&m_ShadowDepthStencilView
		);

		//シェーダーリソースビュー作成
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;//ピクセルフォーマットは32BitFLOAT型
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		SRVDesc.Texture2D.MipLevels = 1;
		m_Device->CreateShaderResourceView(
			depthTexture,
			&SRVDesc,
			&m_ShadowDepthShaderResourceView
		);
	}


	//画面を記録
	{
		//画面バッファ作成
		D3D11_TEXTURE2D_DESC td{};
		td.Width = swapChainDesc.BufferDesc.Width;
		td.Height = swapChainDesc.BufferDesc.Height;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R32_TYPELESS;
		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		td.SampleDesc.Count = 1;// swapChainDesc.SampleDesc;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;
		hr = m_Device->CreateTexture2D(&td, NULL, &m_MainTex);

		// レンダーターゲットビュー作成
		D3D11_RENDER_TARGET_VIEW_DESC rtvd;
		rtvd.Format = td.Format;
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvd.Texture2D.MipSlice = 0;
		hr = m_Device->CreateRenderTargetView(
			m_MainTex,
			&rtvd,
			&m_MainRT
		);

		ID3D11Texture2D* depthStencileTex = NULL;
		D3D11_TEXTURE2D_DESC dstd{};
		dstd.Width = swapChainDesc.BufferDesc.Width;
		dstd.Height = swapChainDesc.BufferDesc.Height;
		dstd.MipLevels = 1;
		dstd.ArraySize = 1;
		dstd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dstd.SampleDesc.Count = 1;// swapChainDesc.SampleDesc;
		dstd.Usage = D3D11_USAGE_DEFAULT;
		dstd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		dstd.CPUAccessFlags = 0;
		dstd.MiscFlags = 0;
		hr = m_Device->CreateTexture2D(
			&dstd,
			NULL,
			&depthStencileTex
		);
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd{};
		dsvd.Format = dstd.Format;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Flags = 0;
		hr=m_Device->CreateDepthStencilView(
			depthStencileTex,
			&dsvd,
			&m_MainDSV
		);

		//シェーダーリソースビュー作成
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
		srvd.Format = DXGI_FORMAT_R32_FLOAT;//ピクセルフォーマットは32BitFLOAT型
		srvd.Format = td.Format;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MostDetailedMip = 0;
		srvd.Texture2D.MipLevels = 1;
		hr=m_Device->CreateShaderResourceView(
			m_MainTex,
			&srvd,
			&m_MainSRV
		);

		;
	}
}



void Renderer::Uninit()
{
	m_WorldBuffer->Release();
	m_ViewBuffer->Release();
	m_ProjectionBuffer->Release();
	m_LightBuffer->Release();
	m_MaterialBuffer->Release();
	m_CameraBuffer->Release();
	m_ParameterBuffer->Release();

	m_ShadowDepthStencilView->Release();
	m_ShadowDepthShaderResourceView->Release();

	m_MainDSV->Release();
	m_MainRT->Release();
	m_MainSRV->Release();
	m_MainTex->Release();

	m_DeviceContext->ClearState();
	m_SwapChain->Release();
	m_DeviceContext->Release();
	m_Device->Release();
	m_RenderTargetView->Release();
	m_DepthStencilView->Release();


	//state
	RSDefault->Release(); 
	RSWireframe->Release();
	RSNoCull->Release();
	RSCullClockWise->Release();

	SSLinear->Release();
	SSAnistropic->Release();

	BSDefault->Release();
	BSNoColorWrite->Release();
	BSTransparent->Release();
	BSAlphaToCoverage->Release();

	DSSDefault->Release();
	DSSMarkMirror->Release();
	DSSDrawReflection->Release();
	DSSNoDoubleBlend->Release();
	DSSNoDepthTest->Release();
	DSSNoDepthWrite->Release();
}




void Renderer::Begin()
{
	//デフォルトのバックバッファと深度バッファへ復帰させておく
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
	float clearColor[4] = { 0.3f, 0.7f, 0.1f, 1.0f };//背景の色
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, clearColor);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::Begin(Camera* cameraComp)
{
	m_DeviceContext->OMSetRenderTargets(1, &cameraComp->MainRT, cameraComp->MainDSV);
	float clearColor[4] = { 0.2f, 0.2f, 1.0f, 1.0f };//背景の色
	m_DeviceContext->ClearRenderTargetView(cameraComp->MainRT, clearColor);
	m_DeviceContext->ClearDepthStencilView(cameraComp->MainDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::Begin(MyLight* lightComp)
{

}

void Renderer::BeginTest()
{
	m_DeviceContext->OMSetRenderTargets(1, &m_MainRT, m_MainDSV);
	float clearColor[4] = { 0.2f, 0.2f, 1.0f, 1.0f };//背景の色
	m_DeviceContext->ClearRenderTargetView(m_MainRT, clearColor);
	m_DeviceContext->ClearDepthStencilView(m_MainDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::BeginDepth()
{
	//シャドウバッファを深度バッファに設定し、内容を1で塗りつぶしておく
	m_DeviceContext->OMSetRenderTargets(0, NULL, m_ShadowDepthStencilView);
	m_DeviceContext->ClearDepthStencilView(m_ShadowDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}



void Renderer::End(bool change)
{
	if (change)
	{
		m_SwapChain->Present(1, 0);
	}
}




void Renderer::SetDepthEnable(bool Enable)
{
	if (Enable)
	{
		m_DeviceContext->OMSetDepthStencilState(DSSDefault, NULL);
	}
	else
	{
		m_DeviceContext->OMSetDepthStencilState(DSSNoDepthTest, NULL);
	}
}

void Renderer::SetAlphaCoverageEnable(bool Enable)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	if (Enable)
	{
		m_DeviceContext->OMSetBlendState(BSAlphaToCoverage, blendFactor, 0xffffffff);
	}
	else
	{
		m_DeviceContext->OMSetBlendState(BSDefault, blendFactor, 0xffffffff);
	}
}

void Renderer::SetWorldViewProjection2D()
{
	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	D3DXMatrixTranspose(&world, &world);
	m_DeviceContext->UpdateSubresource(m_WorldBuffer, 0, NULL, &world, 0, 0);

	D3DXMATRIX view;
	D3DXMatrixIdentity(&view);
	D3DXMatrixTranspose(&view, &view);
	m_DeviceContext->UpdateSubresource(m_ViewBuffer, 0, NULL, &view, 0, 0);

	D3DXMATRIX projection;
	D3DXMatrixOrthoOffCenterLH(&projection, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	D3DXMatrixTranspose(&projection, &projection);
	m_DeviceContext->UpdateSubresource(m_ProjectionBuffer, 0, NULL, &projection, 0, 0);
}

void Renderer::SwitchWVP2D3D(bool to3D)
{
	if (!NowWVP3D && to3D)
		//2D->3D
	{
		NowWVP3D = to3D;

		m_DeviceContext->UpdateSubresource(m_ViewBuffer, 0, NULL, &WVP3DCameraData.ViewMatrix, 0, 0);
		m_DeviceContext->UpdateSubresource(m_ProjectionBuffer, 0, NULL, &WVP3DCameraData.ProjectionMatrix, 0, 0);
	}
	else if (NowWVP3D && !to3D)
		//3D->2D
	{
		NowWVP3D = to3D;
		SetWorldViewProjection2D();
	}
}

void Renderer::SetWorldMatrix(D3DXMATRIX* WorldMatrix)
{
	D3DXMATRIX world;
	D3DXMatrixTranspose(&world, WorldMatrix);
	m_DeviceContext->UpdateSubresource(m_WorldBuffer, 0, NULL, &world, 0, 0);
}

void Renderer::SetViewMatrix(D3DXMATRIX* ViewMatrix)
{
	D3DXMATRIX view;
	D3DXMatrixTranspose(&view, ViewMatrix);
	m_DeviceContext->UpdateSubresource(m_ViewBuffer, 0, NULL, &view, 0, 0);

	WVP3DCameraData.ViewMatrix = view;
}

void Renderer::SetProjectionMatrix(D3DXMATRIX* ProjectionMatrix)
{
	D3DXMATRIX projection;
	D3DXMatrixTranspose(&projection, ProjectionMatrix);
	m_DeviceContext->UpdateSubresource(m_ProjectionBuffer, 0, NULL, &projection, 0, 0);

	WVP3DCameraData.ProjectionMatrix = projection;
}





void Renderer::SetMaterial(MATERIAL Material)
{
	m_DeviceContext->UpdateSubresource(m_MaterialBuffer, 0, NULL, &Material, 0, 0);
}

void Renderer::SetLight(LIGHT Light)
{
	//シェーダー側の都合上で行列を転置しておく
	D3DXMATRIX* lightView = &Light.ViewMatrix;
	D3DXMATRIX* lightProjection = &Light.ProjectionMatrix;
	D3DXMatrixTranspose(lightView, lightView);
	D3DXMatrixTranspose(lightProjection, lightProjection);

	m_DeviceContext->UpdateSubresource(m_LightBuffer, 0, NULL, &Light, 0, 0);
}

void Renderer::SetCamera(const CAMERA& Camera)
{
	//シェーダー側の都合上で行列を転置しておく
	WVP3DCameraData = Camera;
	D3DXMATRIX* view = &WVP3DCameraData.ViewMatrix;
	D3DXMATRIX* projection = &WVP3DCameraData.ProjectionMatrix;
	D3DXMatrixTranspose(view, view);
	D3DXMatrixTranspose(projection, projection);

	m_DeviceContext->UpdateSubresource(m_CameraBuffer, 0, NULL, &WVP3DCameraData, 0, 0);
}

void Renderer::SetParameter(D3DXVECTOR4 Parameter)
{
	m_DeviceContext->UpdateSubresource(m_ParameterBuffer, 0, NULL, &Parameter, 0, 0);
}

void Renderer::CreateVertexShader(
	ID3D11VertexShader** VertexShader, 
	ID3D11InputLayout** VertexLayout,
	const char* FileName,
	D3D11_INPUT_ELEMENT_DESC* LayoutDesc, 
	UINT ElementNum)
{

	FILE* file;
	long int fsize;

	file = fopen(FileName, "rb");
	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	m_Device->CreateVertexShader(buffer, fsize, NULL, VertexShader);




	m_Device->CreateInputLayout(
		LayoutDesc,
		ElementNum,
		buffer,
		fsize,
		VertexLayout
	);

	delete[] buffer;
}

void Renderer::CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName)
{
	FILE* file;
	long int fsize;

	file = fopen(FileName, "rb");
	fsize = _filelength(_fileno(file));
	unsigned char* buffer = new unsigned char[fsize];
	fread(buffer, fsize, 1, file);
	fclose(file);

	m_Device->CreatePixelShader(buffer, fsize, NULL, PixelShader);

	delete[] buffer;
}

#pragma endregion
