#include "common.hlsl"

//法線マップ用のテクスチャ変数を追加
Texture2D g_Texture : register(t0);//通常のテクスチャ用
Texture2D g_TextureNormal : register(t1);//法線マップ用
SamplerState g_SamplerState : register(s0);//これは１つでOK

void main(in PS_IN In, out float4 outDiffuse : SV_Target)

{
	//ピクセルの法線を正規化
	//float4 normal = normalize(In.Normal);

	//このピクセルの法線マップのデータRGBA値を取得
	float4 normalMap = g_TextureNormal.Sample(g_SamplerState, In.TexCoord);// float4(In.Diffuse.rgba);

	//取得したスケーリング済RGBA値をベクトル情報XYZWとして元に戻す
	normalMap = normalMap * 2.0f - 1.0f;

	//outDiffuse = normalMap;
	//return;

		//元に戻したテクスチャRGBAデータを並び替えて法線用変数へ
		float4 normal;
		normal.x = -normalMap.r;
		normal.y = normalMap.b;
		normal.z = normalMap.g;
		normal.w = 0.0f;
		normal = normalize(normal); //正規化


	//光源計算をする
	float light = -dot(normal.xyz, Light.Direction.xyz);

	//スペキュラの計算
	//カメラからピクセルへ向かうベクトル
	float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
	eyev = normalize(eyev); //正規化する

	////光の反射ベクトルを計算
	//float3 refv = reflect(Light.Direction.xyz, normal.xyz);
	//refv = normalize(refv); //正規化する

	//ハーフベクトル作成
	float3	halfv = eyev + Light.Direction.xyz;
	halfv = normalize(halfv); //正規化する


	//鏡面反射の計算
	float specular = -dot(halfv, normal.xyz);
	specular = saturate(specular); //値をサチュレート
	specular = pow(specular, 30); //ここでは３０乗してみる

	//テクスチャのピクセル色を取得
	outDiffuse = 
		g_Texture.Sample(g_SamplerState, In.TexCoord);

	outDiffuse.rgb *=
		In.Diffuse.rgb * light; //明るさと色を乗算
	outDiffuse.a *= 
		In.Diffuse.a; //α別計算

	//スペキュラ値をデフューズとして足しこむ
	outDiffuse.rgb += specular;

}


