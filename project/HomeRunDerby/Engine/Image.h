#pragma once
#include"Sprite.h"
#include"Transform.h"

//�摜�f�[�^�̊Ǘ�
namespace Image
{
	using DirectX::XMVECTOR;

	const int ERROR_CORD	= -1;	//�摜�f�[�^�̎擾�Ɏ��s�����ꍇ�̔ԍ�
	const int DECIMAL_BASE	= 10;	//10�i���̒�

	//�摜�f�[�^�̓ǂݍ���
	//�����FfileName�@	�t�@�C���̃p�X
	//�ߒl�F�n���h���ԍ�
	int Load(const std::string& fileName);

	//�\�������Ԃ�ݒ�
	//�����P�Fhandle		�n���h���ԍ�
	//�����Q�Ftransform		�\��������
	void SetTransform(int handle, Transform& transform);

	//�`��
	//�����Fhandle			�n���h���ԍ�
	void Draw(int handle);

	//�`��
	//�����P�Fhandle�@		�n���h���ԍ�
	//�����Q�Ftype�@		�`��Ɏg�p����V�F�[�_�[�̎��
	void Draw(int handle, Direct3D::SHADER_TYPE type);

	//���R���̕`��
	//�����P:handle[DECIMAL_BASE]	0~9�̐����摜�C���[�W�n���h��
	//�����Q:drawNumber				�`�悷�鐔��
	//�����R:lastDigitTransform		�\�������ԁiposition_�͍ŉ��ʌ��̕`��ʒu�j
	//�����S:intervalWide			�������Ƃ̉���
	void DrawNaturalNumber(int handle[DECIMAL_BASE], int drawNumber, const Transform & lastDigitTransform, float intervalWide);

	//���[�h�ς݉摜�̃n���h���ԍ����擾����
	//�����FfileName�@	�t�@�C���̃p�X
	//�ߒl�F�n���h���ԍ�
	int GetHandle(const std::string& fileName);

	//�����擾
	//�����Fhandle		�n���h���ԍ�
	//�ߒl�F��
	float GetWidth(int handle);

	//�������擾
	//�����Fhandle		�n���h���ԍ�
	//�ߒl�F����
	float GetHeight(int handle);

	//�摜���N���b�N����Ă��邩
	//�����Fhandle		�n���h���ԍ�
	//�ߒl�F����Ă����true�@����Ă��Ȃ����false 
	bool IsClicked(int handle);

	//���
	void Release();
};
