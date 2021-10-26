#pragma once
#include "../Engine/IGameObject.h"
#include "Umpire.h"

const float		CURVE_AMOUNT_ADJUSTMENT = 0.00001f;		//�ω����̕ω��ʂ𒲐����邽�߂̒l

//�{�[���̊Ǘ�
class Ball : public IGameObject
{
	using XMVECTOR = DirectX::XMVECTOR;

private:

	XMVECTOR		movement_;					//�ړ���
	XMVECTOR		rotationAxis_;				//��]��
	float			rotation_;					//��]��
	int				hClliderBall_;				//�{�[���̓����蔻��̃n���h��

public:

	//�R���X�g���N�^
	Ball(IGameObject* parent);

	//������
	void Initialize() override;

	//�{�[���̏�Ԃ�ݒ�
	//�����P�Fmove			�ړ��x�N�g��
	//�����Q�FrotateAxis	��]���x�N�g��
	//�����R�Frotate		��]��
	void SetPitchState(const XMVECTOR & move, const XMVECTOR& rotateAxis, float rotate);

private:

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;

	//�s�b�`���[�ɓ�����ꂽ�{�[���̈ړ�����
	void MovePitchedBall();

	//�ŋ��̈ړ�����
	void MoveHitBall();

	//��]����
	void Rotate();

	//��C��R�͂̎擾
	//�ߒl�F��C��R
	float GetResistance() const;			

	//�d�͂̎擾
	//�ߒl�F�d��
	float GetGravity() const;

	//�}�O�k�X�G�t�F�N�g�̃x�N�g�����擾
	//�ߒl�F�}�O�k�X�G�t�F�N�g�̃x�N�g��
	XMVECTOR GetMagnusEffect() const;

	//���I�u�W�F�N�g�ƏՓ˂������̏���
	//�����P�FpTarget		���������I�u�W�F�N�g
	//�����Q�FpData			�Փˎ��̃f�[�^
	void OnCollision(IGameObject* pTarget, CollisionData *pData) override;

	//�Փˎ��̕����v�Z
	//�����P�FtargetName	�Փ˂����I�u�W�F�N�g�̖��O
	//�����Q�FpData			�Փˎ��̃f�[�^
	void CalculationCollisionPhysics(const std::string& targetName, CollisionData *pData);

	//�Փ˖ʂ̖@���̋t�����ɓ����^���̋z���x�N�g�����擾
	//�����P�Fnormal		�Փ˖ʂ̖@���x�N�g��
	//�����Q�FtargetName	�Փ˂����I�u�W�F�N�g�̖��O
	//�ߒl�@�F�z���x�N�g��
	XMVECTOR GetAbsorption(const XMVECTOR& normal, const std::string& targetName) const;

	//�ړ������ɓ������C�͂��擾
	//�����P�Fnormal	�Փ˖ʂ̖@���x�N�g��
	//�����Q�FtargetName	�Փ˂����I�u�W�F�N�g�̖��O
	//�����R�Ffriction	���C�͂̏o��
	//�ߒl�@�F���C�x�N�g��
	XMVECTOR GetFriction(const XMVECTOR& normal, const std::string& targetName, float* friction) const;

	//�ړ����x�ɉ�������]���̎擾
	//�����Fnormal	�Փ˖ʂ̖@���x�N�g��
	//�ߒl�F��]��
	float GetRotationSpeedAfterCollision(const XMVECTOR& normal) const;

	//�ړ������ɉ�������]���̎擾
	//�����Fnormal		�Փ˖ʂ̖@���x�N�g��
	//�ߒl�F��]���x�N�g��
	XMVECTOR GetRotateAxis(const XMVECTOR& normal) const;
};