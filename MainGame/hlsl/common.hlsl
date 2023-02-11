////////////////////////////////////////////////////////////////////////////////
//���̃t�@�C���͑��̃V�F�[�_�[�t�@�C���փC���N���[�h����܂�
////////////////////////////////////////////////////////////////////////////////


// @brief	�e��}�g���N�X���󂯎��ϐ���p��
cbuffer WorldBuffer : register(b0)//�萔�o�b�t�@ 0 ��
{
	matrix World;
}
cbuffer ViewBuffer : register(b1) //�萔�o�b�t�@ 1 ��
{
	matrix View;
}
cbuffer ProjectionBuffer : register(b2) //�萔�o�b�t�@ 2 ��
{
	matrix Projection;
}
cbuffer MaterialBuffer : register(b3) //�萔�o�b�t�@ 3 ��
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Emission;
	float Shininess;
	float3 Dummy;
}

// @brief	���C�g�I�u�W�F�N�g�\���̂ƃR���X�^���g�o�b�t�@
struct LIGHT
{
	bool  Enable;
	bool3 Dummy; //�z�u�A�h���X��4�̔{���ɂ���ƌ����������̂Œ����p
	float4 Direction; //����C����ł�������VisualStudio������Ă���Ă���B

	float4 Position;
	float4 Angle;//�����̊p�x

	float4 Diffuse;
	float4 Ambient;

	matrix View; //���C�g�J�����̃r���[�s��
	matrix Projection; //���C�g�J�����̃v���W�F�N�V�����s��
};
cbuffer LightBuffer : register(b4)//�R���X�^���g�o�b�t�@�S�ԂƂ���
{
	LIGHT Light; //���C�g�\����
}

// @brief	�J�����I�u�W�F�N�g�\���̂ƃR���X�^���g�o�b�t�@
struct CAMERA
{
	float4 Position; //�J�������W���󂯂Ƃ�ϐ�
	float4 Direction; //����C����ł�������VisualStudio������Ă���Ă���B

	matrix View; //���C�g�J�����̃r���[�s��
	matrix Projection; //���C�g�J�����̃v���W�F�N�V�����s��
};
cbuffer CameraBuffer : register(b5) //�o�b�t�@�̂T�ԂƂ���
{
	CAMERA Camera;
}


// @brief	����p�����[�^
cbuffer ParameterBuffer : register(b6)
{
	float4 Parameter;
}


////////////////////////////////////////////////////////////////////////////////
//���_�V�F�[�_�[�֓��͂����f�[�^���\���̂̌`�ŕ\��
////////////////////////////////////////////////////////////////////////////////

// @brief	���_�o�b�t�@�̓��e���̂���
struct VS_IN
{
	float4 Position : POSITION0; //�Z�}���e�B�N�X�͒��_���C�A�E�g�ɑΉ�
	float4 Normal : NORMAL0;
	float4 Diffuse : COLOR0;
	float2 TexCoord : TEXCOORD0;
	//float3 Offset : Offset;

	uint InstanceID : SV_InstanceID;
};

// @brief	�s�N�Z���V�F�[�_�[�֓��͂����f�[�^���\���̂̌`�ŕ\��
struct PS_IN
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : POSITION0;
	float4 Normal : NORMAL0;
	float4 Diffuse : COLOR0;
	float2 TexCoord : TEXCOORD0;

	float4 ShadowPosition : POSITION1; //���C�g�J�������猩���s�N�Z���̍��W
};

/////////////////////EOF//////////////////////
