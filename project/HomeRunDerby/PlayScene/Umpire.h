#pragma once
#include"../Engine/IGameObject.h"
#include"../Engine/Transform.h"
#include "ProgressFlag.h"

//�R���N���X
class Umpire
{
	using XMVECTOR = DirectX::XMVECTOR;

private:

	ProgressFlag		flag_;				//�����Ɋւ���t���O

public:

	Umpire();
	~Umpire();

	//�Q�[���̏�ԂɊւ���֐�
	void BeginPlay();				//�v���[�J�n
	void EndPlay();					//�v���[�̏I��
	void EndGame();					//�Q�[���̏I��

	bool IsInPlay() const;			//�v���[����
	bool IsHitBall() const;			//�{�[���̏�Ԃ��ŋ���
	bool IsGroundBall() const;		//�{�[���̏�Ԃ��S����
	bool IsFoulBall() const;		//�{�[���̏�Ԃ��t�@�[���{�[����
	bool IsHomeRun() const;			//�z�[��������
	bool IsEndGame() const;			//�Q�[�����I�����邩

	//�{�[���������ƏՓ˂����Ƃ��ɃQ�[��������s��
	//�����P�FcollisionPosision	�Փˈʒu
	//�����Q�FobjectName		�Փ˂����I�u�W�F�N�g�̖��O
	void CollisionBall(const XMVECTOR& collisionPosision, const std::string& objectName);

private:

	//�{�[���ƒn�ʂ������������̏���
	//�����FcollisionPosision	�Փˈʒu
	void CollisionBallToField(const XMVECTOR& collisionPosision);		//�{�[���ƒn�ʂ���������
};