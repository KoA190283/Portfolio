#include "Transform.h"

#include <fstream>

using DirectX::XMMATRIX;
using DirectX::XMVECTOR;

//�R���X�g���N�^
Transform::Transform():pParent_(nullptr)
{
	position_	  = DirectX::XMVectorSet(0, 0, 0, 0);	//�ʒu
	quaternion_	  = DirectX::XMVectorSet(0, 0, 0, 1);	//�N�H�[�^�j�I��
	scale_		  = DirectX::XMVectorSet(1, 1, 1, 0);	//�g�嗦

	matTranslate_ = DirectX::XMMatrixIdentity();		//�ړ��s��
	matRotation_  = DirectX::XMMatrixIdentity();		//��]�s��	
	matScale_	  = DirectX::XMMatrixIdentity();		//�g��s��
}

//�ړ��s����擾
XMMATRIX Transform::GetTranslateMatrix() const
{ 
	return matTranslate_;
}

//��]�s����擾
XMMATRIX Transform::GetRotateMatrix() const
{ 
	return matRotation_;
}			

//�g��s����擾
XMMATRIX Transform::GetScaleMatrix() const
{
	return matScale_; 
}

//���[���h�s����擾
XMMATRIX Transform::GetWorldMatrix()
{
	//���[���h�s����쐬����
	XMMATRIX worldMatrix = matScale_ * matRotation_ * matTranslate_;			//���[���h�s��
	
	//�e��������e�̃��[���h�s���������
	if (pParent_ != nullptr)	worldMatrix *= pParent_->GetWorldMatrix();	

	return worldMatrix;
}

//��]��ݒ�
void Transform::SetRotate(const XMVECTOR & rotate)
{
	quaternion_ = DirectX::XMQuaternionRotationRollPitchYawFromVector(ConvertToRadian(rotate));				//�I�C���[�p���N�H�[�^�j�I����]�ɕϊ�
}

//�e�̕ό`���̐ݒ�
void Transform::SetParent(Transform * pParent)
{
	pParent_ = pParent;
}

//X���̉�]��ǉ�
void Transform::AddRotateX(float x)
{
	XMVECTOR eulerQuat	= DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(x), 0, 0);	//�I�C���[�p���N�H�[�^�j�I����]�ɕϊ�
	quaternion_			= DirectX::XMQuaternionMultiply(eulerQuat, quaternion_);							//�I�C���[�p��]�ƃN�H�[�^�j�I������Z
}

//Y���̉�]��ǉ�
void Transform::AddRotateY(float y)
{
	XMVECTOR eulerQuat	= DirectX::XMQuaternionRotationRollPitchYaw(0, DirectX::XMConvertToRadians(y), 0);	//�I�C���[�p���N�H�[�^�j�I����]�ɕϊ�
	quaternion_			= DirectX::XMQuaternionMultiply(eulerQuat, quaternion_);							//�I�C���[�p��]�ƃN�H�[�^�j�I������Z
}

//Z���̉�]��ǉ�
void Transform::AddRotateZ(float z)
{
	XMVECTOR eulerQuat	= DirectX::XMQuaternionRotationRollPitchYaw(0, 0, DirectX::XMConvertToRadians(z));	//�I�C���[�p���N�H�[�^�j�I����]�ɕϊ�
	quaternion_			= DirectX::XMQuaternionMultiply(eulerQuat, quaternion_);							//�I�C���[�p��]�ƃN�H�[�^�j�I������Z
}

//�e�s��̌v�Z
void Transform::Calclation()
{
	//�ړ��s��
	matTranslate_ = DirectX::XMMatrixTranslation(
		position_.vecX,
		position_.vecY,
		position_.vecZ
	);

	//��]�s��
	matRotation_ = DirectX::XMMatrixRotationQuaternion(quaternion_);

	//�g��k���s��
	matScale_ = DirectX::XMMatrixScaling(scale_.vecX, scale_.vecY, scale_.vecZ);
}

//�I�C���[�p�̊e�p�x��x���@����ʓx�@�ɕϊ�
XMVECTOR Transform::ConvertToRadian(const XMVECTOR& rotate) const
{
	return  DirectX::XMVectorSet(
		DirectX::XMConvertToRadians(rotate.vecX),	//x
		DirectX::XMConvertToRadians(rotate.vecY),	//y
		DirectX::XMConvertToRadians(rotate.vecZ),	//z
		0											//w
		);
}