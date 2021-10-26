#include "ProgressFlag.h"

/*
	ProgressFlag::flag_�̃f�[�^�i�[�ɂ���

	//�r�b�g���F�Ǘ�����t���O

	1:�v���[����
	2:�{�[���ƃo�b�g������������
	3:�t���C�{�[����
	4:�t�B�[���h�͈̔͊O�Ƀ{�[�����o����
	
	5:�z�[��������
	6:�Q�[�����I�����邩
	7:��f�[�^
	8:��f�[�^
*/

const char IN_PLAY			= 0x01;	//�v���[��
const char HIT_BALL			= 0x02;	//�{�[���ƃo�b�g����������
const char GROUND_BALL		= 0x04;	//�O���E���h�ɓ�������
const char FOUL_BALL		= 0x08;	//�t�B�[���h�͈̔͊O�Ƀ{�[�����o��
const char HOME_RUN			= 0x10;	//�z�[������
const char END_GAME			= 0x20;	//�Q�[�����I������
const char ONE_PLAY_ZONE	= 0x1F;	//�����v���[���ƂɎg�p����t���O�͈̔�

ProgressFlag::ProgressFlag() :flag_(0)
{
}

ProgressFlag::~ProgressFlag()
{
}

//�J�n
void ProgressFlag::Play()
{
	flag_ |= IN_PLAY;
}

//�{�[�����o�b�g�ɓ�������
void ProgressFlag::HitBall()
{
	flag_ |= HIT_BALL;
}

//�{�[�����n�ʂɓ�������
void ProgressFlag::GroundBall()
{
	flag_ |= GROUND_BALL;
}

//�t�B�[���h�͈̔͊O�Ƀ{�[�����o��
void ProgressFlag::FoulBall()
{
	flag_ |= FOUL_BALL;
}

//�z�[������
void ProgressFlag::HomeRun()
{
	flag_ |= HOME_RUN;
}

//�Q�[���I��
void ProgressFlag::EndGame()
{
	flag_ |= END_GAME;
}

//�P�v���[���ƂɎg�p����t���O�����Z�b�g
void ProgressFlag::ResetOnePlayFlag()
{
	flag_ &= !ONE_PLAY_ZONE;
}

//�S�Ẵt���O�����Z�b�g
void ProgressFlag::ResetAllFlag()
{
	flag_ = 0;
}

//�v���[����
bool ProgressFlag::IsInPlay() const
{
	return flag_ & IN_PLAY;
}

//�{�[���ƃo�b�g������������
bool ProgressFlag::IsHitBall() const
{
	return flag_ & HIT_BALL;
}

//�ŋ����n�ʂɏՓ˂�����
bool ProgressFlag::IsGroundBall() const
{
	return flag_ & GROUND_BALL;
}

//�t�B�[���h�͈̔͊O�Ƀ{�[�����o����
bool ProgressFlag::IsFoulBall() const
{
	return flag_ & FOUL_BALL;
}

//�z�[��������
bool ProgressFlag::IsHomeRun() const
{
	return flag_ & HOME_RUN;
}

//�Q�[�����I�����邩
bool ProgressFlag::IsEndGame() const
{
	return flag_ & END_GAME;
}
