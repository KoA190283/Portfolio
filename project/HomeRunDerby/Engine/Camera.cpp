#include "Camera.h"

#include "Global.h"

namespace Camera
{
	//�ϐ�
	namespace
	{
		XMVECTOR position_;		//�J�����̈ʒu�i���_�j
		XMVECTOR target_;		//����ʒu�i�œ_�j
		XMMATRIX viewMatrix_;	//�r���[�s��
		XMMATRIX projMatrix_;	//�v���W�F�N�V�����s��
	}

	//������
	void Initialize()
	{
		position_	= DirectX::XMVectorSet(0, 0, -10, 0);																								//�J�����̈ʒu
		target_		= DirectX::XMVectorSet(0, 0, 0, 0);																									//�J�����̏œ_
		projMatrix_ = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, (float)Global::WINDOW_WIDTH / (float)Global::WINDOW_HEIGHT, 0.1f, 200.0f);	//�v���W�F�N�V�����s��
	}
	
	//�X�V
	void Update()
	{
		//�r���[�s��̍쐬
		viewMatrix_ = DirectX::XMMatrixLookAtLH(position_, target_, DirectX::XMVectorSet(0, 1, 0, 0));
	}

	//�ʒu��ݒ�
	void SetPosition(const XMVECTOR &position)
	{
		position_ = position;
	}

	//float������SetPosition
	void SetPosition(float x, float y, float z)
	{
		SetPosition(DirectX::XMVectorSet(x, y, z, 0));
	}

	//�œ_��ݒ�
	void SetTarget(const XMVECTOR &target)
	{
		target_ = target;
	}

	//float������SetTarget
	void SetTarget(float x, float y, float z)
	{
		SetTarget(DirectX::XMVectorSet(x, y, z, 0));
	}

	//�r���[�s����擾
	XMMATRIX GetViewMatrix()
	{
		return viewMatrix_;
	}

	//�v���W�F�N�V�����s����擾
	XMMATRIX GetProjectionMatrix()
	{
		return projMatrix_;
	}

	//�ʒu�̎擾
	XMVECTOR GetPosition()
	{
		return position_;
	}
}