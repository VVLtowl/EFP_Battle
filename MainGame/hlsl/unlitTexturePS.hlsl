//�s�N�Z���V�F�[�_�[
#include "common.hlsl" //�K���C���N���[�h
//
//in �͓��͂���Ă���f�[�^�[
//out �͏o�͂���f�[�^
//

//DirectX�̃e�N�X�`���̐ݒ���󂯌p��
Texture2D g_Texture : register(t0);//�e�N�X�`���[�O��
//DirectX�̃T���v���[�X�e�[�g�ݒ���󂯌p��
SamplerState g_SamplerState : register(s0);//�e�N�X�`���T���v

void main(in PS_IN In, out float4 outDiffuse : SV_Target)
{
	//���̃s�N�Z���Ɏg����e�N�X�`���̐F���擾
	outDiffuse = g_Texture.Sample(g_SamplerState, In.TexCoord);

	//���͂��ꂽ�s�N�Z���̐F�����̂܂܏o��
	outDiffuse *= In.Diffuse;

	////�O���[�X�P�[���ɂ��Ă݂�
	//outDiffuse.rgb = 
	//	  outDiffuse.r * 0.3
	//	+ outDiffuse.g * 0.6
	//	+ outDiffuse.b * 0.1;

	//////�Z�s�A���ϊ����Ă݂�
	//float4 sepia = float4(0.960, 0.784, 0.580, 1);
	//outDiffuse *= sepia;

	////outDiffuse = pow(outDiffuse,6);


	//outDiffuse.a = 1.0f;
}