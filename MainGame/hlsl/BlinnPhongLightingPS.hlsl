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
	float3 eyev = In.WorldPosition.xyz - Camera.Position.xyz;
	eyev = normalize(eyev); //���K������

	////���̔��˃x�N�g�����v�Z
	//float3 refv = reflect(Light.Direction.xyz, normal.xyz);
	//refv = normalize(refv); //���K������

	//�n�[�t�x�N�g���쐬
	float3	halfv = eyev + Light.Direction.xyz;
	halfv = normalize(halfv); //���K������


	//���ʔ��˂̌v�Z
	float specular = -dot(halfv, normal.xyz);
	specular = saturate(specular); //�l���T�`�����[�g
	specular = pow(specular, 30); //�����ł͂R�O�悵�Ă݂�

	//�e�N�X�`���̃s�N�Z���F���擾
	outDiffuse = 
		g_Texture.Sample(g_SamplerState, In.TexCoord);

	//���邳�ƐF����Z

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
		In.Diffuse.a; //���ʌv�Z

	//�X�y�L�����l���f�t���[�Y�Ƃ��đ�������
	outDiffuse.rgb += specular;

}


