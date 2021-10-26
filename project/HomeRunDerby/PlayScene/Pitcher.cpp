#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../ExternalDataManager.h"
#include "Pitcher.h"
#include "PlayScene.h"
#include "Ball.h"

using DirectX::XMVECTOR;

//�R���X�g���N�^
Pitcher::Pitcher(IGameObject* parent)
	: IGameObject(parent, "Pitcher"), nextAnimationFrame_(0), ballType_(-1), aimPosition_(DirectX::g_XMZero)
{
}

//������
void Pitcher::Initialize()
{
	//���f���f�[�^�̃��[�h
	const int MODEL_HANDLE = Model::Load(ExternalDataManager::GetFilePathPitcherModel());
	assert(MODEL_HANDLE != Model::ERROR_CORD);
	
	//�s�b�`���[�v���[�g�̈ʒu���s�b�`���[�̈ʒu
	transform_.position_ = ExternalDataManager::GetPitcherPlatePosition();

	//�A�j���[�V�������ŏ��̏�Ԃɂ���
	ResetAnimation();
}

//�X�V
void Pitcher::Update()
{
	//���݂��v���[���łȂ���Ή������Ȃ�
	PlayScene* pPlayScene = (PlayScene*)pParent_;
	if (!pPlayScene->IsInPlay())	return;

	//���t���[���̎p���ɕό`����
	const int MODEL_HANDLE = Model::GetHandle(ExternalDataManager::GetFilePathPitcherModel());				//�s�b�`���[�̃��f���̃n���h���ԍ�
	Model::DeformAnimationPose(MODEL_HANDLE, nextAnimationFrame_);

	//�s�b�`���[���{�[��������Ă��Ȃ��ꍇ�̏���
	const int RELEASE_FRAME = ExternalDataManager::GetReleaseFrame();										//�s�b�`���[���{�[��������t���[��
	if (nextAnimationFrame_ <= RELEASE_FRAME)
	{
		Ball* pBall = (Ball*)pParent_->FindChildObject("Ball");									//�{�[���I�u�W�F�N�g

		//��̈ʒu���{�[���̈ʒu��
		XMVECTOR drawPosistion = Model::GetBonePosition(MODEL_HANDLE, "PitchHand");				//�������̈ʒu
		transform_.Calclation();																//�s��̍Čv�Z
		drawPosistion = XMVector3TransformCoord(drawPosistion, transform_.GetWorldMatrix());	//���[�J�����W���烏�[���h���W�ɕϊ�
		pBall->SetPosition(drawPosistion);

		//���̃A�j���[�V�����t���[�����{�[��������t���[���Ȃ�{�[���̍X�V��L���ɂ���
		if (nextAnimationFrame_ == RELEASE_FRAME)	pBall->Activate();
	}

	nextAnimationFrame_++;
}

//�`��
void Pitcher::Draw()
{
	//�`�悷�郂�f���̏�Ԃ�ݒ肵�ĕ`��
	const int MODEL_HANDLE = Model::GetHandle(ExternalDataManager::GetFilePathPitcherModel());	//�s�b�`���[�̃��f���̃n���h���ԍ�
	Model::SetTransform(MODEL_HANDLE, transform_);
	Model::Draw(MODEL_HANDLE);
}

//���
void Pitcher::Release()
{
}

//�I������Ă��鋅��̎擾
int Pitcher::GetBallType() const
{
	return ballType_;
}

//�_���ʒu�̎擾
XMVECTOR Pitcher::GetAimPosition() const
{
	return aimPosition_;
}

//�{�[��������ʒu�̎擾
XMVECTOR Pitcher::GetReleasePosition()
{
	//�����[�X���̎p����ݒ�
	const int MODEL_HANDLE = Model::GetHandle(ExternalDataManager::GetFilePathPitcherModel());	//�s�b�`���[�̃��f���̃n���h���ԍ�
	Model::SetAnimationPose(MODEL_HANDLE, ExternalDataManager::GetReleaseFrame());

	//�������̈ʒu���擾
	XMVECTOR releasePosition = Model::GetBonePosition(MODEL_HANDLE, "PitchHand");				//������u�Ԃ̎p�����̓������̈ʒu

	//���[�J�����W���烏�[���h���W�ɕϊ�
	transform_.Calclation();																	//�s��̍Čv�Z
	releasePosition = XMVector3TransformCoord(releasePosition, transform_.GetWorldMatrix());	//�ϊ�

	//�p�������ɖ߂�
	Model::SetAnimationPose(MODEL_HANDLE, nextAnimationFrame_);

	return releasePosition;
}

//�{�[���𓊂��鏀��
void Pitcher::InitializePitch()
{
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::operator*;
	using DirectX::operator*=;
	using DirectX::operator-=;

	//����̑I��
	ballType_ = SelectBallType();

	//������ʒu�̑I��
	aimPosition_ = SelectPitchCourse();

	const int		FAST_BALL	= 0;
	const float		ROTATE		= MinutesToFrame(ExternalDataManager::GetRotate(ballType_));							//rpf�ɕϊ�������]��
	const float		SPEED		= KilometreParHourToMetreParFrame(ExternalDataManager::GetBallSpeed(ballType_));		//mpf�ɕϊ��������x

	//�{�[������������̃{�[���̏�Ԃ�ݒ肷��
	XMVECTOR pitchedMovement	=  ExternalDataManager::GetStrikeZoneCenter() + aimPosition_ - GetReleasePosition();	//�������̃{�[���̈ړ��x�N�g��
	pitchedMovement				=  DirectX::XMVector3Normalize(pitchedMovement);										//�P�ʃx�N�g���ɕϊ�
	pitchedMovement				*= SPEED;																				//�x�N�g���̑傫����ݒ肳��Ă��鋅���ɂ���

	//�I������Ă��鋅�킪�X�g���[�g�ȊO�ł���Εω��ʂ��l�����ăx�N�g����ω�������
	if (ballType_ != FAST_BALL)
	{
		const XMVECTOR	PITCHER_TO_HOME_DIRECTION	= DirectX::XMVector3Normalize(ExternalDataManager::GetHomePlatePosition() - ExternalDataManager::GetPitcherPlatePosition());	//�s�b�`���[����z�[���ւ̒P�ʃx�N�g��
		const XMVECTOR	CURVE_DIRECTION				= DirectX::XMVector3Cross(ExternalDataManager::GetRotateAxis(ballType_), PITCHER_TO_HOME_DIRECTION);							//�ω������̒P�ʃx�N�g��
		const XMVECTOR	RELEASE_TO_HOME_BASE				= ExternalDataManager::GetStrikeZoneCenter() - GetReleasePosition();													//�����[�X�|�C���g����X�g���C�N�]�[���̒��S�ւ̃x�N�g��
		const float		CURVE_AMOUNT				= ROTATE * CURVE_AMOUNT_ADJUSTMENT * RELEASE_TO_HOME_BASE.vecZ;																	//�ω���

		//�{�[���̕ω��Ƌt�����ɕω��ʂ������炷
		pitchedMovement -= CURVE_DIRECTION * CURVE_AMOUNT;
	}

	//�{�[���I�u�W�F�N�g��T��
	Ball* pBall = (Ball*)(pParent_->FindChildObject("Ball"));					

	//�{�[����Ԃ�������u�Ԃ̏�Ԃɐݒ肷��
	pBall->SetPitchState(pitchedMovement, ExternalDataManager::GetRotateAxis(ballType_), ROTATE);

	//�A�j���[�V�����̃t���[����0��
	nextAnimationFrame_ = 0;
}

//�A�j���[�V�������ŏ��̏�Ԃɖ߂�
void Pitcher::ResetAnimation()
{
	//�A�j���[�V�����̃t���[������0�ɂ���
	nextAnimationFrame_		= 0;																//�A�j���[�V�����̃t���[������0��

	//�ŏ��̎p���ɂ���
	const int MODEL_HANDLE	= Model::GetHandle(ExternalDataManager::GetFilePathPitcherModel());	//�X�g���C�N�]�[���̃��f���n���h��
	Model::DeformAnimationPose(MODEL_HANDLE, nextAnimationFrame_);
}

//����̑I��
int Pitcher::SelectBallType() const
{
	//0~1�̗����擾
	const float	RANDOM_NUMBER	= GetRandomPercentage();						//0~1�̗���

	//�擾������������g�p���鋅������߂�
	const int	BALL_TYPE_MAX	= ExternalDataManager::GetBallTypeCount();		//�g�p���̍��v���100%
	float		useRateCount	= 0;											//�m�F�ςݎg�p���̍��v
	for (int ballType = 0; ballType < BALL_TYPE_MAX; ballType++)
	{
		//����̎g�p�����擾���Ċm�F�ς݂̎g�p�����v�ɉ��Z����
		useRateCount += ExternalDataManager::GetUseRateBallType(ballType);

		//���v�����m�F�ςݎg�p���������l�𒴂����璴�������̋�����g�p����
		if (useRateCount >= RANDOM_NUMBER)	return ballType;
	}

	return -1;
}

//�ǂ��ɓ����邩�����߂�
XMVECTOR Pitcher::SelectPitchCourse() const
{
	using DirectX::operator*;

	const float HALF			= 0.5f;													//����
	const float HALF_WIDTH		= ExternalDataManager::GetStrikeZoneWidth() * HALF;		//�X�g���C�N�]�[���̕��̔���
	const float HALF_HEIGHT		= ExternalDataManager::GetStrikeZoneHeight() * HALF;	//�X�g���C�N�]�[���̍����̔���

	//�X�g���C�N�]�[���̒��S����ǂꂾ�����ꂽ�ʒu�ɓ����邩�����߂�
	XMVECTOR pitchCourse	= DirectX::g_XMZero;																	//�X�g���C�N�]�[���̒��S����̋���
	pitchCourse.vecX		= GetRandomPercentage() * ExternalDataManager::GetStrikeZoneWidth() - HALF_WIDTH;		//X����
	pitchCourse.vecY		= GetRandomPercentage() * ExternalDataManager::GetStrikeZoneHeight() - HALF_HEIGHT;		//Y����

	//���݂̃Q�[�����[�h�ɍ��킹�Ē��S����̋����𒲐����Ēl��Ԃ�
	return pitchCourse * ExternalDataManager::GetCoursePercentage();
}

//0~1�̗������擾
float Pitcher::GetRandomPercentage() const
{
	const int RANDOM_NUMBER_WIDTH = 100;					//�����̕�

	return (float)(rand() % RANDOM_NUMBER_WIDTH) / RANDOM_NUMBER_WIDTH;
}

//km/h��m/f�ɒP�ʕϊ�
float Pitcher::KilometreParHourToMetreParFrame(float numeric) const
{
	const float TO_METRE = 1000.f;							//km -> m �̕ϊ�
	const float TO_FRAME = powf(60.f, 3);					//�� -> �t���[���i1/60�b�j�̕ϊ�

	const float TO_METRE_PAR_FRAME = TO_METRE / TO_FRAME;	//km/h -> m/frame �̕ϊ�

	return numeric * TO_METRE_PAR_FRAME;
}

//*/min��*/f�ɒP�ʕϊ�
float Pitcher::MinutesToFrame(float numeric) const
{
	const float TO_FRAME = 1.f / 3600.f;					//min -> f �̕ϊ�

	return numeric * TO_FRAME;
}
