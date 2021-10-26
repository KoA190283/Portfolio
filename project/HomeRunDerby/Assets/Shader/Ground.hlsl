Texture2D		g_texture		: register(t0);	//�e�N�X�`���[
Texture2D		g_normalTexture : register(t1);	//�@���}�b�v�e�N�X�`���[
SamplerState	g_sampler		: register(s0);	//�T���v���[

//�R���X�^���g�o�b�t�@
cbuffer global
{
	float4x4	g_matVertexConversion;	//���_�ϊ��s��
	float4x4	g_matNormalConversion;	//�@���ϊ��s��
	float4x4	g_matWorld;				//���[���h�s��

	float4		g_diffuseColor;			//�}�e���A���̐F
	float4		g_ambient;				//����
	float4		g_specular;				//���ʔ��ˌ�

	float4		g_cameraPosition;		//�J�����̈ʒu
	float		g_shininess;			//����
	bool		g_isTexture;			//�e�N�X�`���̗L��
};

//vs -> ps�̓��o�͍\����
struct VS_OUT
{
	float4 position	: SV_POSITION;	//�ʒu
	float4 eye		: TEXCOORD2;	//����
	float2 uv		: TEXCOORD;		//UV���W
	float4 light	: TEXCOORD1;	//���C�g
};

//���_�V�F�[�_
VS_OUT VS(float4 pos : POSITION, float2 uv : TEXCOORD, float4 normal : NORMAL, float4 tangent : TANGENT)
{
	//�s�N�Z���V�F�[�_�[�֓n������ݒ�
	VS_OUT outData;											//�o�̓f�[�^
	outData.position	= mul(pos, g_matVertexConversion);	//���_�̍��W�𒸓_�ϊ��s��ŕϊ�����
	outData.uv			= uv;

	//�ڐ��E�]�@���E�@��������
	const float3 CROSS_NORMAL_TO_TANGENT = cross((float3)normal, (float3)tangent);											//�]�@���x�N�g��
	float4 binormal;
	binormal.x	= CROSS_NORMAL_TO_TANGENT.x;
	binormal.y	= CROSS_NORMAL_TO_TANGENT.y;
	binormal.z	= CROSS_NORMAL_TO_TANGENT.z;
	binormal.w	= 0;
	tangent.w	= 0;
	normal.w	= 0;

	//�ڐ��E�]�@���E�@����@���ϊ��s��ŕϊ�
	tangent		= mul(tangent, g_matNormalConversion);
	binormal	= mul(binormal, g_matNormalConversion);
	normal		= mul(normal, g_matNormalConversion);

	//���C�g�̃x�N�g���Ɗe���̓��p�����C�g�̃x�N�g���Ƃ��Ĉ���
	float4 light	= float4(0, 1.f, 0.f, 0);								//���C�g�̃x�N�g��
	light			= normalize(light);										//���K��
	outData.light.x = dot(light, tangent);									//x�Ɛڐ�
	outData.light.y = dot(light, binormal);									//y�Ə]�@��
	outData.light.z = dot(light, normal);									//z�Ɩ@��

	//�����x�N�g���Ɗe���̓��p�����C�g�̃x�N�g���Ƃ��Ĉ���
	float4 eye		= normalize(g_cameraPosition - mul(pos, g_matWorld));	//�����x�N�g��
	outData.eye.x	= dot(eye, tangent);									//x�Ɛڐ�
	outData.eye.y	= dot(eye, binormal);									//y�Ə]�@��
	outData.eye.z	= dot(eye, normal);										//z�Ɩ@��

	return outData;
}

//�s�N�Z���V�F�[�_
float4 PS(VS_OUT inData) : SV_TARGET
{
	//���C�g�̃x�N�g���𐳋K��
	float4 LIGHT_DIRECTION = normalize(inData.light);

	//�e�N�X�`���̎g�p�������e����100�{����
	//�g��ϊ��ɂ������L�΂��΍�
	float2 uv = inData.uv * 100.f;

	//�@���x�N�g���̎擾
	float4 normal	= g_normalTexture.Sample(g_sampler, uv) * 2.f - 1.f;	//�e�N�X�`���̒l��(0~1)����(-1~1)��
	normal.w		= 0;													//w�͎g��Ȃ�					
	normal			= normalize(normal);									//���K��

	//�e�N�X�`���̗L�����m�F���ĕ`��s�N�Z����̃I�u�W�F�N�g�̐F��ݒ肷��
	float4 id;																//�I�u�W�F�N�g�̐F
	if (g_isTexture == true) id = g_texture.Sample(g_sampler, uv);			//�e�N�X�`�����g�p���Ă���΃e�N�X�`���F
	else					 id = g_diffuseColor;							//�g�p���Ă��Ȃ���΃}�e���A���F

	//�g�U���ˌ��̎擾
	const float4 SHADE		= saturate(dot(LIGHT_DIRECTION, normalize(normal)));						//���邳
	const float4 DIFFUSE	= SHADE * id;																//�g�U���ˌ�

	//���ʔ��ˌ��̎擾
	float4 specular = float4(0, 0, 0, 0);																//���ʔ��ˌ�
	if (g_specular.a != 0)																				//�X�y�L�����[���̗L��΃X�y�L�����[����ݒ肷��
	{
		const float4 REFLECT	= reflect(LIGHT_DIRECTION, normal);										//�����˃x�N�g��
		specular				= pow(saturate(dot(REFLECT, inData.eye)), g_shininess) * g_specular;	//�n�C���C�g�����߂�
	}

	//�o�͐F�̌v�Z
	const float DARKEN	= 0.7f;											//�Â��F�ɒ������邽�߂̒l
	float4		color	= (g_ambient + DIFFUSE + specular) * DARKEN;	//�o�͐F
	color.a				= 1.f;											//�A���t�@�l�i�����x�j��1

	return color;
}