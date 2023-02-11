//頂点シェーダー
#include "common.hlsl" //必ずインクルード
//
//in は入力されてくるデーター
//out は出力するデータ
//
void main(in VS_IN In, out PS_IN Out)
{
	//１頂点分のデータを受け取り、処理して、出力する
	//頂点変換処理 この処理は必ず必要
	matrix wvp; //行列変数を作成
	wvp = mul(World, View);							//1パス目はライトカメラの行列で
	wvp = mul(wvp, Projection);					//2パス目は通常カメラの行列が設定される
	Out.Position = mul(In.Position, wvp); //変換結果を出力する

	//頂点法線をワールド行列で回転させる(頂点と同じ回転をさせる)
	float4 worldNormal, normal; //ローカル変数を作成
	//	normal = float4(In.Normal.xyz, 0.0);//法線ベクトルのwを0としてコピー（平行移動しないため)
	normal = float4(In.Normal.xyz, 0.0);//法線ベクトルのwを0としてコピー（平行移動しないため)
	worldNormal = mul(normal, World); //法線をワールド行列で回転する
	worldNormal = normalize(worldNormal); //回転後の法線を正規化する
	Out.Normal = worldNormal; //回転後の法線出力 In.Normalでなく回転後の法線を出力

	//光源処理
	float light = -dot(worldNormal, Light.Direction.xyz);
	light = saturate(light);
	Out.Diffuse = In.Diffuse * light;
	Out.Diffuse.a = In.Diffuse.a;
	Out.Position = mul(In.Position, wvp); //頂点座標をワールド変換して出力
	Out.Normal = worldNormal; //法線を出力
	Out.TexCoord = In.TexCoord; //テクスチャ座標を出力


	//ライトの行列を使って頂点を変換して出力する
	matrix lightwvp;
	lightwvp = mul(World, Light.View); //ワールド行列＊ライトビュー行列
	lightwvp = mul(lightwvp, Light.Projection);//さらに＊ライトプロジェクション行列
	Out.ShadowPosition = mul(In.Position, lightwvp);//ライトから見た頂点座標出力
}
