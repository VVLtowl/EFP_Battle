#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
	//ピクセルの法線を正規化
	float4 normal = normalize(In.Normal);
	//光源計算をする
	float light = -dot(normal.xyz, Light.Direction.xyz);

	//スペキュラの計算
	//カメラからピクセルへ向かうベクトル
	float3 eyev = In.WorldPosition.xyz - Camera.Position.xyz;
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

	//明るさと色を乗算

	//outDiffuse.rgb *= light;
	if (light < 0)
	{
		outDiffuse.rgb = Light.Ambient.rgb;
		outDiffuse.rgb *= (-light);
	}
	else
	{
		outDiffuse.rgb *= In.Diffuse.rgb;
		outDiffuse.rgb *= light;
		outDiffuse.rgb *= Light.Diffuse.rgb;
		//outDiffuse.rgb *= In.Diffuse.rgb * Light.Diffuse.rgb * light;
	}
	
	outDiffuse.a *= 
		In.Diffuse.a; //α別計算

	//スペキュラ値をデフューズとして足しこむ
	outDiffuse.rgb += specular;

}


