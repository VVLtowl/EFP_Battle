////////////////////////////////////////////////////////////////////////////////
//このファイルは他のシェーダーファイルへインクルードされます
////////////////////////////////////////////////////////////////////////////////


// @brief	各種マトリクスを受け取る変数を用意
cbuffer WorldBuffer : register(b0)//定数バッファ 0 番
{
	matrix World;
}
cbuffer ViewBuffer : register(b1) //定数バッファ 1 番
{
	matrix View;
}
cbuffer ProjectionBuffer : register(b2) //定数バッファ 2 番
{
	matrix Projection;
}
cbuffer MaterialBuffer : register(b3) //定数バッファ 3 番
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Emission;
	float Shininess;
	float3 Dummy;
}

// @brief	ライトオブジェクト構造体とコンスタントバッファ
struct LIGHT
{
	bool  Enable;
	bool3 Dummy; //配置アドレスを4の倍数にすると効率がいいので調整用
	float4 Direction; //実はC言語でも同じでVisualStudioがやってくれている。

	float4 Position;
	float4 Angle;//視錐の角度

	float4 Diffuse;
	float4 Ambient;

	matrix View; //ライトカメラのビュー行列
	matrix Projection; //ライトカメラのプロジェクション行列
};
cbuffer LightBuffer : register(b4)//コンスタントバッファ４番とする
{
	LIGHT Light; //ライト構造体
}

// @brief	カメラオブジェクト構造体とコンスタントバッファ
struct CAMERA
{
	float4 Position; //カメラ座標を受けとる変数
	float4 Direction; //実はC言語でも同じでVisualStudioがやってくれている。

	matrix View; //ライトカメラのビュー行列
	matrix Projection; //ライトカメラのプロジェクション行列
};
cbuffer CameraBuffer : register(b5) //バッファの５番とする
{
	CAMERA Camera;
}


// @brief	特殊パラメータ
cbuffer ParameterBuffer : register(b6)
{
	float4 Parameter;
}


////////////////////////////////////////////////////////////////////////////////
//頂点シェーダーへ入力されるデータを構造体の形で表現
////////////////////////////////////////////////////////////////////////////////

// @brief	頂点バッファの内容そのもの
struct VS_IN
{
	float4 Position : POSITION0; //セマンティクスは頂点レイアウトに対応
	float4 Normal : NORMAL0;
	float4 Diffuse : COLOR0;
	float2 TexCoord : TEXCOORD0;
	//float3 Offset : Offset;

	uint InstanceID : SV_InstanceID;
};

// @brief	ピクセルシェーダーへ入力されるデータを構造体の形で表現
struct PS_IN
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION0;
	float4 Normal : NORMAL0;
	float4 Diffuse : COLOR0;
	float2 TexCoord : TEXCOORD0;

	float4 ShadowPosition : POSITION1; //ライトカメラから見たピクセルの座標
};

/////////////////////EOF//////////////////////
