#pragma once
#include "../Engine/IGameObject.h"

//�s�b�`���[�̊Ǘ�
class Pitcher : public IGameObject
{
	using XMVECTOR = DirectX::XMVECTOR;

private:

	int			nextAnimationFrame_;	//���݂̃A�j���[�V�����̃t���[����
	int			ballType_;				//�I������Ă��鋅��̔ԍ�
	XMVECTOR	aimPosition_;			//�_���ʒu

public:

	//�R���X�g���N�^
	Pitcher(IGameObject* parent);

	//������
	void Initialize() override;

	//�擾�֐�
	int			GetBallType() const;			//�I������Ă��鋅��
	XMVECTOR	GetAimPosition() const;			//�_���ʒu
	XMVECTOR	GetReleasePosition();			//�{�[��������ʒu

	//�{�[���𓊂��鏀��
	void InitializePitch();

	//�A�j���[�V�������ŏ��̏�Ԃɖ߂�
	void ResetAnimation();

private:

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;

	//�ǂ��ɓ����邩�����߂�
	//�߂�l�F�I�����ꂽ�ʒu���X�g���C�N�]�[���̒��S����ǂꂾ������Ă��邩
	XMVECTOR SelectPitchCourse() const;

	//����̑I��
	//�ߒl�F�I�����ꂽ����̔ԍ�
	int SelectBallType() const;

	//0~1�̗������擾
	//�ߒl�F0~1�̗���
	float GetRandomPercentage() const;

	//km/h��m/f�ɒP�ʕϊ�
	float KilometreParHourToMetreParFrame(float numeric) const;

	//*/min��*/f�ɒP�ʕϊ�
	float MinutesToFrame(float numeric) const;
};