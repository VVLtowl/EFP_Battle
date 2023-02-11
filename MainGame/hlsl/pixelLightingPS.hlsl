//thsozai ==>�O��܂ł̏�ԁ@ZIP�@�@�p�X���[�h�@nabe


//BlinnPhongLightingPS.hlsl
//BlinnPhongLightingVS.hlsl


#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
	//�s�N�Z���̖@���𐳋K��
	float4 normal = normalize(In.Normal);
	//�����v�Z������
	float light = -dot(normal.xyz, Light.Direction.xyz);

	//�X�y�L�����̌v�Z
	//�J��������s�N�Z���֌������x�N�g��
	float3 eyev = In.WorldPosition.xyz - CameraPosition.xyz;
	eyev = normalize(eyev); //���K������
	//���̔��˃x�N�g�����v�Z
	float3 refv = reflect(Light.Direction.xyz, normal.xyz);
	refv = normalize(refv); //���K������
	
	float specular = -dot(eyev, refv); //���ʔ��˂̌v�Z
	specular = saturate(specular); //�l���T�`�����[�g
	specular = pow(specular, 20); //�����ł͂R�O�悵�Ă݂�

	//�e�N�X�`���̃s�N�Z���F���擾
	outDiffuse = 
		g_Texture.Sample(g_SamplerState, In.TexCoord);

	outDiffuse.rgb *=
		In.Diffuse.rgb * light; //���邳�ƐF����Z
	outDiffuse.a *= 
		In.Diffuse.a; //���ʌv�Z

	//�X�y�L�����l���f�t���[�Y�Ƃ��đ�������
	outDiffuse.rgb += specular;

}


