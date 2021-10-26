#pragma once
#include <DirectXMath.h>

#include "Direct3D.h"

//�J����
namespace Camera
{
	using DirectX::XMMATRIX;
	using DirectX::XMVECTOR;

	//�������i�v���W�F�N�V�����s��쐬�j
	void Initialize();

	//�X�V�i�r���[�s��쐬�j
	void Update();

	//���_�i�J�����̈ʒu�j��ݒ�
	//�����Fposition�@XMVECTOR�^�̃J�������W
	void SetPosition(const XMVECTOR &position);

	//���_�i�J�����̈ʒu�j��ݒ�
	//�����Fx�@x���̃J�������W
	//�����Fy�@y���̃J�������W
	//�����Fz�@z���̃J�������W
	void SetPosition(float x, float y, float z);

	//�œ_�i����ʒu�j��ݒ�
	//�����Fposition�@XMVECTOR�^�̃J�����œ_
	void SetTarget(const XMVECTOR &target);

	//�œ_�i����ʒu�j��ݒ�
	//�����Fx�@x���̃J�����œ_
	//�����Fy�@y���̃J�����œ_
	//�����Fz�@z���̃J�����œ_
	void SetTarget(float x, float y, float z);

	//�擾�֐�
	XMMATRIX GetViewMatrix();		//�r���[�s��
	XMMATRIX GetProjectionMatrix();	//�v���W�F�N�V�����s��
	XMVECTOR GetPosition();			//�ʒu
};

