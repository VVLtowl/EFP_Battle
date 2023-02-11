//頂点シェーダー
#include "common.hlsl" //必ずインクルード

// @brief	インスタンスの情報
struct INSTANCE_PARAM
{
	matrix World:World;
	float4 Color:Color;
	//uint InstanceID : SV_InstanceID;
	//matrix Transform;
};
StructuredBuffer<INSTANCE_PARAM> InstanceParams:register(t1);


void main(
	in VS_IN In,
	//in INSTANCE_PARAM InInstance,
	out PS_IN Out)
{
	//１頂点分のデータを受け取り、処理して、出力する
	//頂点変換処理 この処理は必ず必要
	matrix wvp; //行列変数を作成
	float3 d = { 1,1,1 };
	//In.Position.xyz += d;
	//In.Position = mul(In.Position, InstanceParams[In.InstanceID].World);
	//In.Position.x += In.InstanceID * 0.5f;
	wvp = mul(InstanceParams[In.InstanceID].World, World);
	wvp = mul(wvp, View); //wvp = ワールド行列＊カメラ行列
	wvp = mul(wvp, Projection); //wvp = wvp *プロジェクション行列
	Out.Position = mul(In.Position, wvp); //変換結果を出力する
	//受け取ったこの頂点のデフューズをそのまま出力
	//Out.Diffuse = In.Diffuse;
	Out.Diffuse = InstanceParams[In.InstanceID].Color;
	//テクスチャ座標を出力
	Out.TexCoord = In.TexCoord;
}