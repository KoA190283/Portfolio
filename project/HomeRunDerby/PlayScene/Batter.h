#pragma once
#include <map>
#include "../Engine/IGameObject.h"
#include "../ExternalDataManager.h"

//�o�b�^�[�̊Ǘ�
class Batter : public IGameObject
{
	using XMVECTOR		= DirectX::XMVECTOR;
	using SWING_COURSE	= ExternalDataManager::SWING_COURSE;

private:

	XMVECTOR						prevBatPosition_;		//�O�t���[���̃o�b�g�̈ʒu
	std::map<SWING_COURSE, float>	compositeMap_;			//�����Ɏg�p������ <��������R�[�X, �����䗦>
	int								nextAnimationFrame_;	//���ɕ`�悷��A�j���[�V�����̃t���[����
	int								hModelDraw_;			//�`�悷�郂�f���̃n���h��
	int								hColliderBat_;			//�o�b�g�̓����蔻��̃n���h��
	bool							isSwing_;				//�X�C���O���s����

public:

	//�R���X�g���N�^
	Batter(IGameObject* parent);

	//������
	void Initialize() override;

	//�I�u�W�F�N�g�ɑ΂��ăX�C���O���^����e���i�x�N�g���j���擾����
	//�����FhitPosition�@�X�C���O�̉e�������󂯂�I�u�W�F�N�g�̈ʒu
	//�ߒl�FhitPosition�̈ʒu�ɂ���I�u�W�F�N�g�ɑ΂��ė^����X�C���O�̃x�N�g��
	XMVECTOR GetSwingVector(const XMVECTOR& hitPosition) const;

	//swingPosition�ɃX�C���O���邽�߂ɕK�v�ȏ����擾���Ď���Update����X�C���O����
	//�����FswingPosition�@�X�C���O����ʒu(�X�g���C�N�]�[���̐^�񒆊)
	void Swing(const XMVECTOR& swingPosition);

	//�A�j���[�V�������ŏ��̏�Ԃɖ߂�
	void ResetAnimation();

private:

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;


	//�X�g���C�N�]�[���̐ݒ�
	void SetStrikeZone() const;

	//�o�b�g�̓����蔻��̈ʒu�̐ݒ�
	void SetBatColliderPosition();

	//���t���[���̃A�j���[�V�����̎p����ݒ肷��
	void SetNextAnimationPose();

	//��̒l�̒��Ԓl(���ϒl)���擾
	//�����P�Fvalue1�@��ڂ̒l
	//�����Q�Fvalue2�@��ڂ̒l
	//�ߒl�@�F���Ԓl(���ϒl)
	float GetMiddleValue(float value1, float value2) const;

	//��̒l�̂��傫���ق��̒l���擾
	//�����P�Fvalue1�@��ڂ̒l
	//�����Q�Fvalue2�@��ڂ̒l
	//�ߒl�@�F�傫���ق��̒l
	float GetLargerValue(float value1, float value2) const;

	//��̒l�̂�菬�����ق��̒l���擾
	//�����P�Fvalue1�@��ڂ̒l
	//�����Q�Fvalue2�@��ڂ̒l
	//�ߒl�@�F�������ق��̒l
	float GetSmallerValue(float value1, float value2) const;
};