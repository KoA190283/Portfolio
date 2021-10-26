#pragma once
#include <DirectXMath.h>

#define vecX m128_f32[0]	//x
#define vecY m128_f32[1]	//y
#define vecZ m128_f32[2]	//z
#define vecW m128_f32[3]	//w

//�ʒu�A�����A�g�嗦�Ȃǂ��Ǘ�����N���X
class Transform
{
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;

private:

	XMMATRIX	matTranslate_;					//�ړ��s��
	XMMATRIX	matRotation_;					//��]�s��	
	XMMATRIX	matScale_;						//�g��s��
	Transform*	pParent_;						//�e�I�u�W�F�N�g��Transform

public:

	XMVECTOR position_;							//�ʒu
	XMVECTOR quaternion_;						//�N�H�[�^�j�I����]
	XMVECTOR scale_;							//�g�嗦

public:

	//�R���X�g���N�^
	Transform();

	////////////////////
	//�擾�֐�
	XMMATRIX GetTranslateMatrix() const;		//�ړ��s����擾
	XMMATRIX GetRotateMatrix() const;			//��]�s����擾
	XMMATRIX GetScaleMatrix() const;			//�g��s����擾
	XMMATRIX GetWorldMatrix();					//���[���h�s����擾

	
	////////////////////
	//�ݒ�֐�
	void SetRotate(const XMVECTOR & rotate);	//��]��ݒ�@			rotate�F�I�C���[�p
	void SetParent(Transform * pParent);		//�e�̕ό`���̐ݒ�	pParent	�e�̕ό`���̃|�C���^

	////////////////////
	//�ǉ��֐�
	void AddRotateX(float x);				//X���̉�]��ǉ�	x:x���̉�]�ʁi�x���j
	void AddRotateY(float y);				//Y���̉�]��ǉ�	y:y���̉�]�ʁi�x���j
	void AddRotateZ(float z);				//Z���̉�]��ǉ�	z:z���̉�]�ʁi�x���j


	//�e�s��̌v�Z
	void Calclation();

private:

	//�I�C���[�p�̊e�p�x��x���@����ʓx�@�ɕϊ�
	//�����Frotate	�x���@�̊p�x
	//�ߒl�F�ʓx�@�̊p�x
	XMVECTOR ConvertToRadian(const XMVECTOR& rotate) const;
};