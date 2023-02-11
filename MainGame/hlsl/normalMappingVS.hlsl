//頂点シェーダー
#include "common.hlsl"
void main(in VS_IN In, out PS_IN Out)
{
	//wvpワールドビュープロジェクション行列作成
	matrix wvp;
	wvp = mul(World, View);// = ワールド行列*ビュー行列
	wvp = mul(wvp, Projection);// = wvp * プロジェクション行列
	//法線をワールド行列で変換
	float4 worldNormal, normal;
	normal = float4(In.Normal.xyz, 0.0);//wを0として法線をコピー
	worldNormal = mul(normal, World);// =法線*ワールド行列
	worldNormal = normalize(worldNormal); //正規化する
	//頂点座標をwvpで変換して出力
	Out.Position = mul(In.Position, wvp);// = In.Position*wvp
	//頂点座標をワールド行列で変換して出力
	Out.WorldPosition = mul(In.Position, World);
	//ワールド変換した法線を出力
	Out.Normal = worldNormal;
	//デフューズを出力
	Out.Diffuse = In.Diffuse;
	//テクスチャ座標を出力
	Out.TexCoord = In.TexCoord;
}
