Texture2D		g_texture		: register(t0);	//�e�N�X�`���[
SamplerState	g_sampler		: register(s0);	//�T���v���[

//�R���X�^���g�o�b�t�@
cbuffer global
{
	float4x4	g_matVertexConversion;		//���_���W�ϊ��s��
	float		g_time;						//�o�ߎ���
};

//vs -> ps�̓��o�͍\����
struct VS_OUT
{
	float4 position : SV_POSITION;	//�ʒu
	float2 uv		: TEXCOORD;   	//UV���W
};

//���_�V�F�[�_
VS_OUT VS(float4 position : POSITION, float2 uv : TEXCOORD)
{
	//�s�N�Z���V�F�[�_�[�֓n������ݒ�
	VS_OUT outData;
	outData.position	= mul(position, g_matVertexConversion);
	outData.uv			= uv;

	//�܂Ƃ߂ďo��
	return outData;
}

//�s�N�Z���V�F�[�_
float4 PS(VS_OUT inData) : SV_TARGET
{
	//uv���W�������ɂ��炷
	float2 uv		= float2(inData.uv.x + g_time, inData.uv.y - g_time * 0.2f);

	return g_texture.Sample(g_sampler, uv);
}