Texture2D		g_texture : register(t0);	//�e�N�X�`���[
SamplerState	g_sampler : register(s0);	//�T���v���[

//�R���X�^���g�o�b�t�@
cbuffer global
{
	float4x4	g_matVertexConversion;	//���_�ϊ��s��
	float4x4	g_matNormalConversion;	//�@���ϊ��s��
	float4x4	g_matWorld;				//���[���h�s��

	float4		g_diffuseColor;			//�}�e���A���̐F
	float4		g_ambient;				//����
	float4		g_specular;				//���ʔ��ˌ�

	float4		g_camPos;				//�J�����̈ʒu
	float		g_shininess;			//����
	bool		g_isTexture;			//�e�N�X�`���̗L��
};

//vs -> ps�̓��o�͍\����
struct VS_OUT
{
	float4 position	: SV_POSITION;	//�ʒu
	float4 normal	: NORMAL;		//�@��
	float4 eye		: TEXCOORD;		//����
	float2 uv		: TEXCOORD1;	//UV���W
};

//���_�V�F�[�_
VS_OUT VS(float4 position : POSITION, float2 uv : TEXCOORD, float4 normal : NORMAL)
{
	VS_OUT outData;															//�o�̓f�[�^
	outData.position	= mul(position, g_matVertexConversion);				//���_�̍��W�𒸓_�ϊ��s��ŕϊ�����
	outData.normal		= mul(normal, g_matNormalConversion);				//���_�̖@����@���ϊ��p�s��ŕϊ�����
	outData.eye			= normalize(g_camPos - mul(position, g_matWorld));	//���_���王�_�ւ̃x�N�g��
	outData.uv			= uv;												//�e�N�X�`���̓\��t���Ɏg�p������W

	return 	outData;
}

//�s�N�Z���V�F�[�_
float4 PS(VS_OUT inData) : SV_Target
{
	//�e�N�X�`���̗L�����m�F���ĕ`��s�N�Z����̃I�u�W�F�N�g�̐F��ݒ肷��
	float4 diffuse;																	//�I�u�W�F�N�g�̐F
	if (g_isTexture == true) diffuse = g_texture.Sample(g_sampler, inData.uv);		//�e�N�X�`�����g�p���Ă���΃e�N�X�`���F
	else					 diffuse = g_diffuseColor;								//�g�p���Ă��Ȃ���΃}�e���A���F

	return diffuse;
}