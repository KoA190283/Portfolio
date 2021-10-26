#include "../Engine/Global.h"
#include "../ExternalDataManager.h"
#include "../InternalPlayDataReservoir.h"
#include "Umpire.h"

using DirectX::XMVECTOR;

Umpire::Umpire()
{
}

Umpire::~Umpire()
{
}

//�v���[�J�n
void Umpire::BeginPlay()
{
	flag_.Play();
}

//�����v���[�̏I��
void Umpire::EndPlay()
{
	//�v���[���łȂ���Ή������Ȃ�
	if (!flag_.IsInPlay())	return;

	//�z�[�������ł���Γ��_��ǉ�����
	const int ADD_SCORE = 1;												//�ǉ����链�_��
	if (flag_.IsHomeRun())	InternalPlayDataReservoir::AddScore(ADD_SCORE);	

	//�P�v���[���ƂɎg�p����t���O�����Z�b�g
	flag_.ResetOnePlayFlag();								

	//�c��񐔂��P���炷
	InternalPlayDataReservoir::SubtractRemainingCount();						
}

//�Q�[���̏I��
void Umpire::EndGame()
{
	flag_.EndGame();
}

//�v���[�������擾����
bool Umpire::IsInPlay() const
{
	return flag_.IsInPlay();
}

//�{�[���̏�Ԃ��ŋ������擾����
bool Umpire::IsHitBall() const
{
	return flag_.IsHitBall();
}

//�{�[���̏�Ԃ��S�������擾����
bool Umpire::IsGroundBall() const
{
	return flag_.IsGroundBall();
}

//�t�@�[���{�[�������擾����
bool Umpire::IsFoulBall() const
{
	return flag_.IsFoulBall();
}

//�z�[�����������擾����
bool Umpire::IsHomeRun() const
{
	return flag_.IsHomeRun();
}

//�Q�[�����I�����邩���擾����
bool Umpire::IsEndGame() const
{
	return flag_.IsEndGame();
}

//�{�[���������ƏՓ˂����Ƃ��ɃQ�[��������s��
void Umpire::CollisionBall(const XMVECTOR & collisionPosision, const std::string& objectName)
{
	//�{�[�����Փ˂����I�u�W�F�N�g���ɉ���������
	if (objectName == "Field")			CollisionBallToField(collisionPosision);	//�t�B�[���h�ɓ�������
	if (objectName == "Batter")			flag_.HitBall();							//�o�b�^�[�i�o�b�g�j�ɓ�������
}

//�{�[���ƒn�ʂ������������̏���
void Umpire::CollisionBallToField(const XMVECTOR & collisionPosision)
{
	using DirectX::operator-;

	if (flag_.IsGroundBall())	return;				//�{�[�����n�ʂɗ����Ă���Ή������Ȃ�

	//�ڒn�ς݂̏�ԂɕύX
	flag_.GroundBall();								

	//�z�[������̒��S���ƏՓ˒n�_�ւ̃x�N�g���̊p�x�����߂�
	const XMVECTOR	CENTER_LINE		= DirectX::XMVector3Normalize(ExternalDataManager::GetPitcherPlatePosition() - ExternalDataManager::GetHomePlatePosition());	//�z�[������̒��S��
	const XMVECTOR	BALL_DIRECTION	= DirectX::XMVector3Normalize(collisionPosision);																				//�{�[���̕���
	const float		DOT_BALL_CENTER	= DirectX::XMVector3Dot(CENTER_LINE, BALL_DIRECTION).vecX;																		//�z�[���x�[�X����Z���^�[�����̐��ƃ{�[���̕����̓��p
	const float		FAIR_ANGLE_MAX	= 0.5f;																															//�t�F�A���肳���ő�p�x

	//�t�B�[���h�͈̔͊O�ɏo�Ă�����X�R�A�ɑ΂���e���͂Ȃ�
	const bool IS_FOUL_BALL = DOT_BALL_CENTER < FAIR_ANGLE_MAX;							//�t�B�[���h�͈̔͊O�ɏo�Ă��邩
	if (IS_FOUL_BALL)
	{
		flag_.FoulBall();
		return;	
	}

	//�z�[�������ɂȂ�������擾����
	const float	DISTANCE_BALL		= DirectX::XMVector3Length(collisionPosision).vecX;	//�{�[���܂ł̋���
	const float HOME_RUN_DISTANCE	= ExternalDataManager::GetHomeRunDistance();		//�z�[�������ɂȂ鋗��
	
	//�z�[�������Ȃ瓾�_�t���O��true�ɂ���
	const bool IS_HOME_RUN = DISTANCE_BALL > HOME_RUN_DISTANCE;							//�z�[��������
	if (IS_HOME_RUN)	flag_.HomeRun();
}