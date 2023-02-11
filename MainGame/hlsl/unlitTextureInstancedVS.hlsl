//���_�V�F�[�_�[
#include "common.hlsl" //�K���C���N���[�h

// @brief	�C���X�^���X�̏��
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
	//�P���_���̃f�[�^���󂯎��A�������āA�o�͂���
	//���_�ϊ����� ���̏����͕K���K�v
	matrix wvp; //�s��ϐ����쐬
	float3 d = { 1,1,1 };
	//In.Position.xyz += d;
	//In.Position = mul(In.Position, InstanceParams[In.InstanceID].World);
	//In.Position.x += In.InstanceID * 0.5f;
	wvp = mul(InstanceParams[In.InstanceID].World, World);
	wvp = mul(wvp, View); //wvp = ���[���h�s�񁖃J�����s��
	wvp = mul(wvp, Projection); //wvp = wvp *�v���W�F�N�V�����s��
	Out.Position = mul(In.Position, wvp); //�ϊ����ʂ��o�͂���
	//�󂯎�������̒��_�̃f�t���[�Y�����̂܂܏o��
	//Out.Diffuse = In.Diffuse;
	Out.Diffuse = InstanceParams[In.InstanceID].Color;
	//�e�N�X�`�����W���o��
	Out.TexCoord = In.TexCoord;
}