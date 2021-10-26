#pragma once
#include "../Engine/IGameObject.h"
#include "../Engine/IUserInterface.h"
#include "Umpire.h"

//�v���C�V�[���̊Ǘ�
class PlayScene : public IGameObject
{
	using XMVECTOR = DirectX::XMVECTOR;

private:

	std::list<IUserInterface*>	userInterfaceList_;	//UI�N���X�̃��X�g
	Umpire*						pUmpire_;			//�R���N���X
	int							timer_;				//�o�ߎ��Ԃ��v��

public:
	//�R���X�g���N�^
	PlayScene(IGameObject* parent);

	//������
	void Initialize() override;

	//�{�[���������ƏՓ˂����Ƃ��̏���
	//�����P�FcollisionPosision	�Փˎ��̃{�[���̈ʒu
	//�����Q�FobjectName		�Փ˂����I�u�W�F�N�g�̖��O
	void CollisionBall(const XMVECTOR& collisionPosision, const std::string& objectName);

	//�Q�[�����̑��x���擾
	//�ߒl�F�Q�[�����̉����x�i1���������x�j
	float GetGameSpeedAcceleration() const;

	//�v���[�̏�Ԃ��擾
	bool IsInPlay() const;		//�v���[����
	bool IsHitBall() const;		//�ŋ���
	bool IsHomeRun() const;		//�z�[��������

private:

	//�X�V
	void Update() override;

	//�`��
	void Draw() override;

	//���
	void Release() override;

	//�q���̕`����s������ɍs���`�揈��
	void DrawAfterChildren() override;

	//�i�s�󋵂��X�V
	void UpdateProgress();

	//�v���[�J�n
	void BeginPlay();

	//�v���[�I��
	void EndPlay();

	//�J�������ŏ��̈ʒu�ɖ߂�
	void ResetCamera() const;
};