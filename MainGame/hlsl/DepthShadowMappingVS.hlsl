//���_�V�F�[�_�[
#include "common.hlsl" //�K���C���N���[�h
//
//in �͓��͂���Ă���f�[�^�[
//out �͏o�͂���f�[�^
//
void main(in VS_IN In, out PS_IN Out)
{
	//�P���_���̃f�[�^���󂯎��A�������āA�o�͂���
	//���_�ϊ����� ���̏����͕K���K�v
	matrix wvp; //�s��ϐ����쐬
	wvp = mul(World, View);							//1�p�X�ڂ̓��C�g�J�����̍s���
	wvp = mul(wvp, Projection);					//2�p�X�ڂ͒ʏ�J�����̍s�񂪐ݒ肳���
	Out.Position = mul(In.Position, wvp); //�ϊ����ʂ��o�͂���

	//���_�@�������[���h�s��ŉ�]������(���_�Ɠ�����]��������)
	float4 worldNormal, normal; //���[�J���ϐ����쐬
	//	normal = float4(In.Normal.xyz, 0.0);//�@���x�N�g����w��0�Ƃ��ăR�s�[�i���s�ړ����Ȃ�����)
	normal = float4(In.Normal.xyz, 0.0);//�@���x�N�g����w��0�Ƃ��ăR�s�[�i���s�ړ����Ȃ�����)
	worldNormal = mul(normal, World); //�@�������[���h�s��ŉ�]����
	worldNormal = normalize(worldNormal); //��]��̖@���𐳋K������
	Out.Normal = worldNormal; //��]��̖@���o�� In.Normal�łȂ���]��̖@�����o��

	//��������
	float light = -dot(worldNormal, Light.Direction.xyz);
	light = saturate(light);
	Out.Diffuse = In.Diffuse * light;
	Out.Diffuse.a = In.Diffuse.a;
	Out.Position = mul(In.Position, wvp); //���_���W�����[���h�ϊ����ďo��
	Out.Normal = worldNormal; //�@�����o��
	Out.TexCoord = In.TexCoord; //�e�N�X�`�����W���o��


	//���C�g�̍s����g���Ē��_��ϊ����ďo�͂���
	matrix lightwvp;
	lightwvp = mul(World, Light.View); //���[���h�s�񁖃��C�g�r���[�s��
	lightwvp = mul(lightwvp, Light.Projection);//����Ɂ����C�g�v���W�F�N�V�����s��
	Out.ShadowPosition = mul(In.Position, lightwvp);//���C�g���猩�����_���W�o��
}
