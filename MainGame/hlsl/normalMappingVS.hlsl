//���_�V�F�[�_�[
#include "common.hlsl"
void main(in VS_IN In, out PS_IN Out)
{
	//wvp���[���h�r���[�v���W�F�N�V�����s��쐬
	matrix wvp;
	wvp = mul(World, View);// = ���[���h�s��*�r���[�s��
	wvp = mul(wvp, Projection);// = wvp * �v���W�F�N�V�����s��
	//�@�������[���h�s��ŕϊ�
	float4 worldNormal, normal;
	normal = float4(In.Normal.xyz, 0.0);//w��0�Ƃ��Ė@�����R�s�[
	worldNormal = mul(normal, World);// =�@��*���[���h�s��
	worldNormal = normalize(worldNormal); //���K������
	//���_���W��wvp�ŕϊ����ďo��
	Out.Position = mul(In.Position, wvp);// = In.Position*wvp
	//���_���W�����[���h�s��ŕϊ����ďo��
	Out.WorldPosition = mul(In.Position, World);
	//���[���h�ϊ������@�����o��
	Out.Normal = worldNormal;
	//�f�t���[�Y���o��
	Out.Diffuse = In.Diffuse;
	//�e�N�X�`�����W���o��
	Out.TexCoord = In.TexCoord;
}