#pragma once

//�Q�[���i�s�Ɋւ���t���O���Ǘ�����N���X
class ProgressFlag
{
private:

	char flag_;					//�Q�[���i�s�Ɋւ���t���O

public:

	ProgressFlag();
	~ProgressFlag();

	//�e�t���O�̕ύX
	void Play();					//�J�n
	void HitBall();					//�{�[�����o�b�g�ɓ�������
	void GroundBall();				//�{�[�����n�ʂɓ�������
	void FoulBall();				//�t�B�[���h�͈̔͊O�Ƀ{�[�����o��
	void HomeRun();					//�z�[������
	void EndGame();					//�Q�[���I��
	void ResetOnePlayFlag();		//�P�v���[���ƂɎg�p����t���O�����Z�b�g
	void ResetAllFlag();			//�S�Ẵt���O�����Z�b�g

	//�e�t���O�̊m�F
	bool IsInPlay() const;			//�v���[����
	bool IsHitBall() const;			//�{�[���ƃo�b�g������������
	bool IsGroundBall() const;		//�ŋ����n�ʂɏՓ˂�����
	bool IsFoulBall() const;		//�t�B�[���h�͈̔͊O�Ƀ{�[�����o����
	bool IsHomeRun() const;			//�z�[��������
	bool IsEndGame() const;			//�Q�[�����I�����邩
};