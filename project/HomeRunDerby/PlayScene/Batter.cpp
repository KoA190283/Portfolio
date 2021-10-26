#include "../Engine/CapsuleCollider.h"
#include "../Engine/Model.h"
#include "../Engine/Input.h"
#include "../Engine/Math.h"
#include "../ExternalDataManager.h"
#include "Batter.h"
#include "PlayScene.h"


//�R���X�g���N�^
Batter::Batter(IGameObject* parent)
	: IGameObject(parent, "Batter"), prevBatPosition_(DirectX::g_XMZero), nextAnimationFrame_(0), hModelDraw_(-1), hColliderBat_(-1), isSwing_(0)
{
}

//������
void Batter::Initialize()
{
	//���f���f�[�^�̃��[�h
	for (int i = 0; i < (int)SWING_COURSE::MAX; i++)
	{
		const int MODEL_HANDLE = Model::Load(ExternalDataManager::GetFilePathBatterModel((SWING_COURSE)i));
		assert(MODEL_HANDLE != Model::ERROR_CORD);
	}

	//�A�j���[�V�������ŏ��̏�Ԃɂ���
	ResetAnimation();

	//�X�g���C�N�]�[���̐ݒ���s��
	SetStrikeZone();
	
	//�����蔻��̐ݒ�ǉ�
	const XMVECTOR	COLLIDER_RELATIVE_POSITIONS[2]	= { DirectX::g_XMZero,DirectX::g_XMZero };							//�I�u�W�F�N�g���瓖���蔻��ւ̏����x�N�g��
	const XMVECTOR	COLLIDER_SIZE					= DirectX::g_XMOne * ExternalDataManager::GetBatRadius();			//�����蔻��̃T�C�Y
	CapsuleCollider* newCollider					= new CapsuleCollider(COLLIDER_RELATIVE_POSITIONS, COLLIDER_SIZE);	//�V���������蔻��
	hColliderBat_ = AddCollider(newCollider);																			//�Q�[���I�u�W�F�N�g�ɓ����蔻���ǉ�
	SetBatColliderPosition();																							//�����蔻��̈ʒu��ݒ�
}

//�X�V
void Batter::Update()
{
	//�����蔻��̈ʒu���Đݒ�
	SetBatColliderPosition();

	if (!isSwing_)	return;		//�X�C���O���Ă��Ȃ���Έȍ~�̏����͍s��Ȃ�

	//�A�j���[�V�����̃t���[����i�߂�
	nextAnimationFrame_++;

	//���t���[���̃A�j���[�V�����̎p����ݒ肷��
	SetNextAnimationPose();

	//�A�j���[�V�������I�����Ă��Ȃ���Έȉ��̏������s��Ȃ�
	const int ANIMATION_FRAME_MAX = 60;
	if (nextAnimationFrame_ < ANIMATION_FRAME_MAX)	return;
	
	//�A�j���[�V�����������̏�Ԃɖ߂�
	ResetAnimation();
}

//�`��
void Batter::Draw()
{
	//�`�悷�郂�f���̏�Ԃ�ݒ肵�ĕ`��
	Model::SetTransform(hModelDraw_, transform_);
	Model::Draw(hModelDraw_);
}

//���
void Batter::Release()
{
}

//�I�u�W�F�N�g�ɑ΂��ăX�C���O���^����e���i�x�N�g���j���擾����
DirectX::XMVECTOR Batter::GetSwingVector(const XMVECTOR& hitPosition) const
{
	using DirectX::operator-;
	using DirectX::operator*;

	const XMVECTOR	BAT_CENTER_POSITION	= Model::GetBonePosition(hModelDraw_, "batCenter");				//�o�b�g�̒��S�i�c�j�̈ʒu
	const XMVECTOR	BAT_GRIP_POSITION	= Model::GetBonePosition(hModelDraw_, "batGrip");				//�o�b�g�̃O���b�v�̈ʒu
	const XMVECTOR	BAT_TIP_POSITION	= Model::GetBonePosition(hModelDraw_, "batTip");				//�o�b�g�̐�[�̈ʒu

	//�X�C���O�̌����x�N�g���̎擾
	XMVECTOR swingDirection = DirectX::XMVector3Normalize(BAT_CENTER_POSITION - prevBatPosition_);		//�X�C���O�̌����x�N�g��
	{
		//���E�����ւ̃x�N�g������������������
		swingDirection.vecX *= 0.8f;

		//�������ɂȂ�Ȃ��悤�ɒ���
		const float ADJUST_VECTOR_Y_MIN = 0.3f;
		if (swingDirection.vecY < ADJUST_VECTOR_Y_MIN)		swingDirection.vecY = ADJUST_VECTOR_Y_MIN;

		//������������Ȃ��悤�ɒ���
		const float ADJUST_VECTOR_Y_MAX = 0.6f;
		if (swingDirection.vecY > ADJUST_VECTOR_Y_MAX)		swingDirection.vecY = ADJUST_VECTOR_Y_MAX;

		//���ʕ����ւ̃x�N�g���������傫������
		swingDirection.vecZ -= 0.3f;

		//���K�����ĒP�ʃx�N�g���ɂ���
		swingDirection = DirectX::XMVector3Normalize(swingDirection);
	}

	//�͂̑傫���̎擾
	const float		DISTANCE_MAX				= DirectX::XMVector3Length(BAT_GRIP_POSITION - BAT_TIP_POSITION).vecX + ExternalDataManager::GetBatRadius();		//�o�b�g�ƃI�u�W�F�N�g�̋����̍ő�l
	const float		DISTANCE_TO_HIT_POSITION	= DirectX::XMVector3Length(BAT_CENTER_POSITION - hitPosition).vecX;													//�o�b�g�̐c�ƃI�u�W�F�N�g�̋���
	const float		DISTANCE_PERCENTAGE			= 0.5f + 0.5f * DISTANCE_TO_HIT_POSITION / DISTANCE_MAX;															//�I�u�W�F�N�g�ɗ^����Ռ��̊���
	const float		ADJUST_POWER_IMPACT_FRAME	= 0.05f;																											//�ő�l�t���[��������1�t���[������邲�Ƃɂǂꂾ���͂������邩
	const float		IMPACT_PERCENTAGE			= 1.f / (1.f + ADJUST_POWER_IMPACT_FRAME * abs(nextAnimationFrame_ - ExternalDataManager::GetBatterImpactFrame()));	//���t���[���̗͂̊���
	
	return swingDirection * ExternalDataManager::GetBatterPower() * DISTANCE_PERCENTAGE * IMPACT_PERCENTAGE;
}

//swingPosition�ɃX�C���O���邽�߂ɕK�v�ȏ����擾���Ď���Update����X�C���O����
void Batter::Swing(const XMVECTOR& swingPosition)
{
	using DirectX::operator-;
	using DirectX::XMVectorSet;


	if (isSwing_) return;											//�X�C���O���Ȃ牽�����Ȃ�

	const XMVECTOR	SWING_POSITION		= swingPosition - ExternalDataManager::GetStrikeZoneCenter();	//�X�C���O�ʒu�i�X�g���C�N�]�[���̒��S�����_�Ɂj
	const float		HALF				= 0.5f;															//����
	const float		HALF_HEIGHT			= ExternalDataManager::GetStrikeZoneHeight() * HALF;			//�X�g���C�N�]�[���̔����̍���
	const float		HALF_WIDTH			= ExternalDataManager::GetStrikeZoneWidth() * HALF;				//�X�g���C�N�]�[���̔����̉���
	const int		POLYGON_VERTICES	= 3;															//�O�p�|���S���Ɋ܂܂�钸�_��
	const int		POLYGON_COUNT		= 4;															//�X�g���C�N�]�[���̃|���S����
	const int		NEED_COURSE_MAX		= 2;															//�X�C���O�A�j���[�V�����̍����ɕK�v�ȃR�[�X��
	
	const XMVECTOR CORNER_POSITION[(int)SWING_COURSE::MAX] =		//�X�g���C�N�]�[���̒��_�̍��W		
	{
		XMVectorSet( HALF_WIDTH, HALF_HEIGHT, 0, 0),				//����
		XMVectorSet( HALF_WIDTH,-HALF_HEIGHT, 0, 0),				//����
		XMVectorSet(-HALF_WIDTH, HALF_HEIGHT, 0, 0),				//�E��
		XMVectorSet(-HALF_WIDTH,-HALF_HEIGHT, 0, 0),				//�E��
		DirectX::g_XMZero,											//���S
	};

	const SWING_COURSE INDEX[POLYGON_COUNT][POLYGON_VERTICES] =		//���S�Ɗp�̎O�p�`�C���f�b�N�X
	{
		{SWING_COURSE::LEFT_HIGH , SWING_COURSE::RIGHT_HIGH, SWING_COURSE::CENTER},		//��
		{SWING_COURSE::RIGHT_LOW , SWING_COURSE::LEFT_LOW  , SWING_COURSE::CENTER},		//��
		{SWING_COURSE::LEFT_LOW  , SWING_COURSE::LEFT_HIGH , SWING_COURSE::CENTER},		//��
		{SWING_COURSE::RIGHT_HIGH, SWING_COURSE::RIGHT_LOW , SWING_COURSE::CENTER},		//�E
	};

	//�X�g���C�N�]�[���̒��S�Ǝl���̎O�p�`�̒��ɃX�C���O�ʒu������O�p�`��T���A
	//���S�����̒��_�Ɍ������x�N�g�����g�p���ăX�C���O�ʒu�ɍs�����߂ɕK�v�Ȋ��������߁A
	//�����Ɏg�p����f�[�^�Ƃ��Ċi�[����
	for (int polygon_i = 0; polygon_i < POLYGON_COUNT; polygon_i++)
	{
		//�X�g���C�N�]�[���̒��S�Ɗp�����ԎO�p�`�̒���SWING_POSITION�������Ă��邩�𔻒�
		const bool IS_IN_POLYGON = Math::IntersectLineToPolygon(SWING_POSITION, XMVectorSet(0, 0, -1.f, 0), 
			CORNER_POSITION[(int)INDEX[polygon_i][0]], CORNER_POSITION[(int)INDEX[polygon_i][1]], CORNER_POSITION[(int)INDEX[polygon_i][2]], nullptr);

		//�����ĂȂ���΂��̐�̏����͍s��Ȃ��Ƀ��[�v�̐擪��
		if (!IS_IN_POLYGON)	continue;

		float percentages[POLYGON_COUNT - 1];		//�X�g���C�N�]�[���̒��S����|���S���̊p�Ɍ������x�N�g���̔�

		//�X�g���C�N�]�[���̒��S����|���S���̊p�ւ̃x�N�g�����g�p���ăX�C���O�ʒu�ɍs�����߂ɕK�v�Ȋ��������߂�
		Math::IntersectLineToLine2D(CORNER_POSITION[(int)SWING_COURSE::CENTER], SWING_POSITION,
			CORNER_POSITION[(int)INDEX[polygon_i][0]], -CORNER_POSITION[(int)INDEX[polygon_i][1]], &percentages[0], &percentages[1], nullptr);

		//compositeMap_�Ɋi�[����
		compositeMap_.emplace(INDEX[polygon_i][0], percentages[0]);
		compositeMap_.emplace(INDEX[polygon_i][1], percentages[1]);

		break;
	}

	isSwing_ = true;
}

//�A�j���[�V�������ŏ��̏�Ԃɖ߂�
void Batter::ResetAnimation()
{
	nextAnimationFrame_ = 0;																					//���t���[����0��
	isSwing_			= false;																				//�X�C���O���Ă��Ȃ�
	hModelDraw_			= Model::GetHandle(ExternalDataManager::GetFilePathBatterModel(SWING_COURSE::CENTER));	//�`�悷�郂�f���𒆐S��U��A�j���[�V������
	AllColliderActivate();																						//�����蔻���L���ɂ���
	compositeMap_.clear();																						//�A�j���[�V���������Ɏg�p����f�[�^���N���A
	Model::DeformAnimationPose(hModelDraw_, nextAnimationFrame_);												//�`�悷�郂�f�����A�j���[�V�����̍ŏ��̎p���ɂ���
}

//�X�g���C�N�]�[���̐ݒ�
void Batter::SetStrikeZone() const
{
	/*
		�X�g���C�N�]�[���̍����͍\�������̎p������Ɍ��肷��

		����F���ƍ����ق��̌��̍����̒���
		�����F�Ⴂ�ق��̕G�̍���
	*/

	//�X�g���C�N�]�[���̍����ݒ�ɕK�v�ȑ̂̕��ʂ̍������擾
	const float LEFT_SHOULDER_HEIGHT	  = Model::GetBonePosition(hModelDraw_, "leftShoulder").vecY;		//�����̍���
	const float RIGHT_SHOULDER_HEIGHT	  = Model::GetBonePosition(hModelDraw_, "rightShoulder").vecY;		//�E���̍���
	const float SHOULDER_HEIGHT			  = GetLargerValue(LEFT_SHOULDER_HEIGHT, RIGHT_SHOULDER_HEIGHT);	//�����ق������̍����Ƃ��Ĉ���
	
	const float LEFT_KNEE_HEIGHT		  = Model::GetBonePosition(hModelDraw_, "leftKnee").vecY;			//���G�̍���
	const float RIGHT_KNEE_HEIGHT		  = Model::GetBonePosition(hModelDraw_, "rightKnee").vecY;			//�E�G�̍���
	const float KNEE_HEIGHT				  = GetSmallerValue(LEFT_KNEE_HEIGHT, RIGHT_KNEE_HEIGHT);			//�Ⴂ�ق���G�̍����Ƃ��Ĉ���

	const float WAIST_HEIGHT			  = Model::GetBonePosition(hModelDraw_, "waist").vecY;				//���̍���

	//�X�g���C�N�]�[���̍����̏���Ɖ������擾
	const float UPPER_LIMIT				  = GetMiddleValue(SHOULDER_HEIGHT, WAIST_HEIGHT);					//�X�g���C�N�]�[���̍����̏��
	const float LOWER_LIMIT				  = KNEE_HEIGHT;													//�X�g���C�N�]�[���̍����̉���

	//�X�g���C�N�]�[���̒��S�̍����ƑS�̂̍������擾
	const float STRIKE_ZONE_CENTER_HEIGHT = GetMiddleValue(UPPER_LIMIT, LOWER_LIMIT);						//�X�g���C�N�]�[���̒��S�̍���
	const float STRIKE_ZONE_HEIGHT		  = UPPER_LIMIT - LOWER_LIMIT;										//�X�g���C�N�]�[���̍����͈�

	//�X�g���C�N�]�[���̍�����ݒ�
	ExternalDataManager::SetStrikeZoneHeight(STRIKE_ZONE_HEIGHT, STRIKE_ZONE_CENTER_HEIGHT);
}

//�����蔻��̈ʒu�̐ݒ�
void Batter::SetBatColliderPosition()
{
	//�o�b�g�̗��[�̈ʒu��ݒ�
	XMVECTOR bothEndPosition[2] = { Model::GetBonePosition(hModelDraw_,"batGrip"),Model::GetBonePosition(hModelDraw_,"batTip") };
	SetColliderPosition(hColliderBat_, bothEndPosition);
}

//���t���[���̃A�j���[�V�����̎p����ݒ肷��
void Batter::SetNextAnimationPose()
{
	//�ύX�O�̃o�b�g�̈ʒu���擾
	prevBatPosition_ = Model::GetBonePosition(hModelDraw_, "batCenter");

	//���S��U��A�j���[�V���������t���[���̎p���ɂ���
	const int MODEL_HANDLE_CENTER = Model::GetHandle(ExternalDataManager::GetFilePathBatterModel(SWING_COURSE::CENTER));						//���S���X�C���O����o�b�^�[�̃��f���̃n���h���ԍ�
	Model::SetAnimationPose(MODEL_HANDLE_CENTER, nextAnimationFrame_);

	//���S��U��A�j���[�V�����Ɗp��U��A�j���[�V�������������ăA�j���[�V���������R�ɂ���
	//���S��U��A�j���[�V�����Ɗp��U��A�j���[�V�����̎��t���[���̎p���̍������p��U��A�j���[�V�����̐V�����p���Ƃ��Đݒ�
	for (auto it : compositeMap_)
	{
		const SWING_COURSE	COMPOSITE_COURCE				= it.first;																			//��������X�C���O�R�[�X
		const float			COMPOSITE_PERCENTAGE			= it.second;																		//���S��U��A�j���[�V��������ǂꂾ���p��U��A�j���[�V�����Ɋ񂹂邩
		const int			MODEL_HANDLE_COMPOSITE_COURCE	= Model::GetHandle(ExternalDataManager::GetFilePathBatterModel(COMPOSITE_COURCE));	//��������R�[�X���X�C���O����o�b�^�[�̃��f���̃n���h���ԍ�

		//��������A�j���[�V���������t���[���̎p���ɂ���
		Model::SetAnimationPose(MODEL_HANDLE_COMPOSITE_COURCE, nextAnimationFrame_);

		//��̎p���̍�����V�����p���Ƃ��Đݒ�
		Model::SetDifferencePose(MODEL_HANDLE_COMPOSITE_COURCE, MODEL_HANDLE_CENTER, COMPOSITE_PERCENTAGE);
	}

	//�ݒ肵���p���̍����𒆐S��U��A�j���[�V�����̎��t���[���̎p���ƍ������Ďw�肵���ʒu�ɃX�C���O����A�j���[�V�����̎��t���[���̎p�������
	for (auto it : compositeMap_)
	{
		const SWING_COURSE	COMPOSITE_COURCE				= it.first;																			//��������X�C���O�R�[�X
		const int			MODEL_HANDLE_COMPOSITE_COURCE	= Model::GetHandle(ExternalDataManager::GetFilePathBatterModel(COMPOSITE_COURCE));	//��������R�[�X���X�C���O����o�b�^�[�̃��f���̃n���h���ԍ�

		//��̎p���̍�����V�����p���Ƃ��Đݒ�
		Model::AddDifferencePose(MODEL_HANDLE_CENTER, MODEL_HANDLE_COMPOSITE_COURCE);
	}

	//���������p���Ƀ��f����ό`������
	Model::Deform(MODEL_HANDLE_CENTER);

	//�`�悷�郂�f���������������f���ɐݒ肷��
	hModelDraw_ = MODEL_HANDLE_CENTER;
}

//��̒l�̒��Ԓl(���ϒl)���擾
float Batter::GetMiddleValue(float value1, float value2) const
{
	const float HALF = 0.5f;				//����
	return (value1 + value2) * HALF;
}

//��̒l�̂��傫���ق��̒l���擾
float Batter::GetLargerValue(float value1, float value2) const
{
	return (value1 > value2) ? value1 : value2;
}

//��̒l�̂�菬�����ق��̒l���擾
float Batter::GetSmallerValue(float value1, float value2) const
{
	return (value1 < value2) ? value1 : value2;
}
