#include "common.hlsl"

Texture2D g_Texture : register(t0);
Texture2D g_TextureShadowDepth : register(t1);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
	//日向での色＝普通のテクスチャ＊頂点色を作成しておく
	outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);
	outDiffuse *= In.Diffuse;

	//xyz/w -> 2D xy square projection, value:(-1~1)
	//3D z -> distance projection, 
	In.ShadowPosition.xyz /= In.ShadowPosition.w; //投影面でのピクセルの座標つくる
	In.ShadowPosition.x = (In.ShadowPosition.x + 1.0f) * 0.5f; //テクスチャ座標に変換
	In.ShadowPosition.y = -(In.ShadowPosition.y - 1.0f) * 0.5f; //Yは符号を反転しておく

	//シャドウマップテクスチャより、ライトカメラからピクセルまでの距離（深度値）を取得
	float depth = g_TextureShadowDepth.Sample(g_SamplerState, In.ShadowPosition.xy);

	//取得値が計算上のピクセルへの距離より小さい
	if (depth < In.ShadowPosition.z - 0.001) //0.01はZファイティング補正値（後述）
	{
		outDiffuse.rgb *= 0.5f;//pow(depth, 3); //色を暗くする
	}
}


