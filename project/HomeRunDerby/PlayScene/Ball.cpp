#include "../Engine/Camera.h"
#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/Transform.h"
#include "../ExternalDataManager.h"
#include "Ball.h"
#include "Batter.h"
#include "Pitcher.h"
#include "PlayScene.h"
#include "StrikeZone.h"

using DirectX::XMVECTOR;
using DirectX::operator+=;
using DirectX::operator*=;
using DirectX::operator+;
using DirectX::operator-;
using DirectX::operator*;

//�R���X�g���N�^
Ball::Ball(IGameObject* parent)
	: IGameObject(parent, "Ball"), rotation_(0), hClliderBall_(0)
{
	movement_		= DirectX::XMVectorSet(0, 0, 0, 0);
	rotationAxis_	= DirectX::XMVectorSet(0, 0, 0, 1);
}

//������
void Ball::Initialize()
{
	//���f���f�[�^�̃��[�h
	const int MODEL_HANDLE = Model::Load(ExternalDataManager::GetFilePathBallModel());
	assert(MODEL_HANDLE != Model::ERROR_CORD);

	//�����蔻��̐ݒ�ǉ�
	const XMVECTOR	COLLIDER_RELATIVE_POSITION	= DirectX::g_XMZero;													//�I�u�W�F�N�g���瓖���蔻��ւ̃x�N�g��
	SphereCollider* newCollider					= new SphereCollider(&COLLIDER_RELATIVE_POSITION, transform_.scale_);	//�V���������蔻��
	newCollider->Continuous();																							//�����蔻�肷�蔲���h�~
	AddCollider(newCollider);																							//�Q�[���I�u�W�F�N�g�ɓ����蔻���ǉ�

	//�X�V���~�߂�
	Inactivate();

	//�`����~�߂�
	Invisible();
}

//�X�V
void Ball::Update()
{
	//�{�[�����ŋ��̏�Ԃł���Αŋ��p�̍X�V���s��
	if (((PlayScene*)pParent_)->IsHitBall())	MoveHitBall();

	//�s�b�`���[�ɓ������Ă����Ԃ��ς���Ă��Ȃ���Γ�����ꂽ�{�[���̈ړ��������s��
	else										MovePitchedBall();

	//��]������
	Rotate();
}

//�`��
void Ball::Draw()
{
	//�`�悷�郂�f���̏�Ԃ�ݒ肵�ĕ`��
	const int MODEL_HANDLE = Model::GetHandle(ExternalDataManager::GetFilePathBallModel());	//�{�[���̃��f���n���h��
	Model::SetTransform(MODEL_HANDLE, transform_);
	Model::Draw(MODEL_HANDLE);
}

//���
void Ball::Release()
{
}

//�ŋ��̈ړ�����
void Ball::MoveHitBall()
{
	XMVECTOR moveDiff		=  DirectX::g_XMZero;		//�O�t���[���̈ړ��x�N�g���Ƃ̍���
	moveDiff.vecY			-= GetGravity();			//�d��
	moveDiff				+= GetMagnusEffect();		//�}�O�k�X����
	moveDiff				*= 1.f - GetResistance();	//��C��R

	movement_ += moveDiff;								//�ړ��x�N�g����ω�������
	transform_.position_	+= movement_;				//�ʒu��ύX
}

//�s�b�`���[�ɓ�����ꂽ�{�[���̈ړ�����
void Ball::MovePitchedBall()
{
	Pitcher* pPitcher = (Pitcher*)pParent_->FindChildObject("Pitcher");	//�s�b�`���[�̃Q�[���I�u�W�F�N�g

	const int		BALL_TYPE	= pPitcher->GetBallType();				//�I������Ă��鋅��
	const int		FAST_BALL	= 0;									//�t�@�X�g�{�[���i�X�g���[�g�j

	//�I������Ă��鋅�킪�t�@�X�g�{�[���ȊO�ł���Ή�]�ʂƉ�]�������ɍ��킹�Ĉړ��x�N�g���ɕω���^����
	if (BALL_TYPE != FAST_BALL)
	{
		const XMVECTOR	PITCH_DIRECTION		= DirectX::XMVector3Normalize(ExternalDataManager::GetHomePlatePosition() - ExternalDataManager::GetPitcherPlatePosition());	//�s�b�`���[����z�[���ւ̌����x�N�g��
		const XMVECTOR	CURVE_DIRECTION		= DirectX::XMVector3Cross(rotationAxis_, PITCH_DIRECTION);																		//�ω��������
		const float		CURVE_AMOUNT		= rotation_ * CURVE_AMOUNT_ADJUSTMENT;																							//�ω���

		movement_ += CURVE_DIRECTION * CURVE_AMOUNT * ((PlayScene*)pParent_)->GetGameSpeedAcceleration();
	}

	transform_.position_ += movement_;			//�ʒu��ύX
}

//��]����
void Ball::Rotate()
{
	if (DirectX::XMVector3Equal(rotationAxis_, DirectX::g_XMZero)) return;																//��]����������Ή������Ȃ�

	const float		DEGREE_MAX		= 360.f;																							//�ő�p�x
	const float		ROTATION_DEGREE	= rotation_ * DEGREE_MAX;																			//��]��
	const XMVECTOR  QUATERNION		= DirectX::XMQuaternionRotationAxis(rotationAxis_, DirectX::XMConvertToRadians(ROTATION_DEGREE));	//�t���[���ł̉�]�N�H�[�^�j�I��
	
	transform_.quaternion_			= DirectX::XMQuaternionMultiply(transform_.quaternion_, QUATERNION);								//�O�t���[���܂ł̉�]�Ǝ��t���[���̉�]������
}

//�{�[���̏�Ԃ�ݒ�
void Ball::SetPitchState(const XMVECTOR& move, const XMVECTOR& rotateAxis, float rotate)
{
	const float		ACCELERATION = ((PlayScene*)pParent_)->GetGameSpeedAcceleration();	//�Q�[�����̉����x

	//�I�u�W�F�N�g�̏�Ԃ�ݒ�
	transform_.scale_		= DirectX::g_XMOne * ExternalDataManager::GetBallRadius();
	transform_.quaternion_	= DirectX::XMVectorSet(0, 0, 0, 1);
	movement_				= move * ACCELERATION;
	rotationAxis_			= rotateAxis;
	rotation_				= rotate * ACCELERATION;

	//�����蔻��̐ݒ�
	SetColliderSize(hClliderBall_, transform_.scale_);
}

//���I�u�W�F�N�g�ƏՓ˂������̏���
void Ball::OnCollision(IGameObject* pTarget, CollisionData* pData)
{
	if (!IsActive())	return;													//�X�V���ĂȂ���Ή������Ȃ�

	const std::string	TARGET_NAME				= pTarget->GetName();			//�Փ˂����I�u�W�F�N�g�̖��O
	const XMVECTOR		COLLISION_POSITION_BALL = pData->positionList.front();	//�Փˎ��̃{�[���̈ʒu
	
	//�Փ˂������Ƃ��V�[���I�u�W�F�N�g�ɓ`����
	PlayScene* pPlayScene	= (PlayScene*)pParent_;								//�V�[���I�u�W�F�N�g
	pPlayScene->CollisionBall(COLLISION_POSITION_BALL, pTarget->GetName());

	//�Փ˂����I�u�W�F�N�g���X�g���C�N�]�[���ł���Ε����I�ȕω��͂Ȃ��̂ňȍ~�̏����͍s��Ȃ�
	if (TARGET_NAME == "StrikeZone")	return;

	//�Փˎ��̕����v�Z
	CalculationCollisionPhysics(TARGET_NAME, pData);
}

//��C��R�̎擾
float Ball::GetResistance() const
{
	//�ړ����������0��Ԃ�
	const bool IS_STOP = DirectX::XMVector3Equal(movement_, DirectX::g_XMZero);			//�~�܂��Ă��邩
	if (IS_STOP) return 0.f;															

	const float ACCELERATION	= ((PlayScene*)pParent_)->GetGameSpeedAcceleration();	//�Q�[�����̉����x
	const float	BALL_RADIUS		= ExternalDataManager::GetBallRadius();					//�{�[���̔��a(m)
	const float DENSITY			= 1.1681f;												//��C�̖��x(kg/m^3)
	const float VISCOSITY		= 1.8f * powf(0.1f,5) / Global::GAME_FPS * ACCELERATION;//��C�̔S�x(Pa/f)
	const float SPEED			= DirectX::XMVector3Length(movement_).vecX;				//���x(m/f)
	const float AREA			= powf(BALL_RADIUS, 2) * DirectX::XM_PI;				//�ʐ�(m^2)
	const float REYNOLDS		= DENSITY * SPEED * BALL_RADIUS * 2 / VISCOSITY;		//���C�m���Y��

	float	coefficient;																//�R�͌W��
	if		(REYNOLDS < 2)		coefficient = 24.f / REYNOLDS;							//�w����
	else if	(REYNOLDS < 500)	coefficient = 10.f / sqrtf(REYNOLDS);					//���Ԉ�
	else						coefficient = 0.44f;									//������

	return  0.5f * DENSITY * powf(SPEED, 2) * AREA * coefficient;						//�R��
}

//�d�͂̎擾
float Ball::GetGravity() const
{
	const float TIME_ACCELERATION		= ((PlayScene*)pParent_)->GetGameSpeedAcceleration();	//�Q�[�����̉����x
	const float GRAVITY_ACCELERATION	= 9.80665f;												//�d�͉����x(m/s^2)
	const float TIME					= 1.f / Global::GAME_FPS * TIME_ACCELERATION;			//�ړ�����(s)
	
	return 0.5f * GRAVITY_ACCELERATION * powf(TIME, 2);											//1f������̏d��
}

//�}�O�k�X���ʂ̎擾
XMVECTOR Ball::GetMagnusEffect() const
{
	//�ړ����������0�x�N�g����Ԃ�
	const bool IS_STOP = DirectX::XMVector3Equal(movement_, DirectX::g_XMZero);				//�~�܂��Ă��邩
	if (IS_STOP)		return DirectX::g_XMZero;					

	//��]�����������0�x�N�g����Ԃ�
	const bool EXISTS = DirectX::XMVector3Equal(rotationAxis_, DirectX::g_XMZero);
	if (EXISTS)	return DirectX::g_XMZero;

	const XMVECTOR	EFFECT_DIRECTION = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(rotationAxis_, movement_));			//��]���Ɖ^���x�N�g���̊O�ς̋t�x�N�g��
	const float		DENSITY			 = 1.1681f;																					//��C�̖��x(kg/m^3)
	const float		DIAMETER		 = ExternalDataManager::GetBallRadius() * 2.f;												//�{�[���̒��a(m)
	const float		VELOCITY		 = DirectX::XMVector3Length(movement_).vecX;												//���x(m/f)

	const float		LIFT			 = 1.f / 8.f * powf(DirectX::XM_PI, 2) * DENSITY * powf(DIAMETER, 3) * VELOCITY * rotation_;//�g��

	return EFFECT_DIRECTION * LIFT;																								//1f������̃}�O�k�X����
}

//�Փˎ��̕����v�Z
void Ball::CalculationCollisionPhysics(const std::string& targetName, CollisionData* pData)
{
	//�Փˌ�̈ړ��x�N�g��
	float				friction				= 0.f;														//���C��
	const XMVECTOR		COLLISION_POSITION_BALL = pData->positionList.front();								//�Փˎ��̃{�[���̈ʒu
	const XMVECTOR		NORMAL_DIRECTION		= DirectX::XMVector3Normalize(-pData->direction);			//�Փ˖ʂ̖@���x�N�g��
	const XMVECTOR		FRICTION				= GetFriction(NORMAL_DIRECTION, targetName, &friction);		//���C
	const XMVECTOR		REFLECT					= DirectX::XMVector3Reflect(movement_, NORMAL_DIRECTION);	//�{�[���̔��˃x�N�g��
	const XMVECTOR		ABSORPTION				= GetAbsorption(NORMAL_DIRECTION, targetName);				//�z��������

	XMVECTOR			newMovement				= REFLECT + ABSORPTION + FRICTION;							//�Փ˔����̈ړ��x�N�g��

	//�Փ˂����I�u�W�F�N�g�����o�b�^�[�Ȃ�X�C���O�̉e���𔽉f����
	if (targetName == "Batter")
	{
		Batter* pBatter = (Batter*)pParent_->FindChildObject("Batter");										//�o�b�^�[�̃I�u�W�F�N�g

		const XMVECTOR	SWING_VECTOR		= pBatter->GetSwingVector(COLLISION_POSITION_BALL);				//�{�[���ɑ΂���X�C���O�̃x�N�g��
		const XMVECTOR	SWING_DRECTION		= DirectX::XMVector3Normalize(SWING_VECTOR);					//�X�C���O�̌����x�N�g��
		const float		DOT_SWING_NORMAL	= DirectX::XMVector3Dot(SWING_DRECTION, NORMAL_DIRECTION).vecX;	//�X�C���O�̌����x�N�g���ƏՓ˖ʂ̖@���x�N�g���̓��p
		const float		PERCENTAGE			= 0.5f + 0.5f * DOT_SWING_NORMAL;								//�X�C���O�̉e����
		const float		ACCELERATION		= ((PlayScene*)pParent_)->GetGameSpeedAcceleration();			//�Q�[�����̉����x
		
		//�X�C���O�̃x�N�g�����Փ˖ʂ̖@���x�N�g���Ƃ̊p�x�ƃQ�[�����̉����x�ɍ��킹�đ傫����ς��ă{�[���̈ړ��x�N�g���ɔ��f����
		newMovement += SWING_VECTOR * PERCENTAGE * ACCELERATION;
	}

	//�Î~����
	const bool IS_STOP = DirectX::XMVector3Length(newMovement).vecX < friction;				//�^���̗͂Ɩ��C�͂ǂ��炪�傫����
	if (IS_STOP) newMovement = DirectX::g_XMZero;											//�Î~���Ă�����^���ʂ�0�ɂ���

	//�n�ʂւ̂߂荞�ݖh�~
	if (targetName == "Field")																//�Փ˂����I�u�W�F�N�g���O���E���h�Ȃ�
		if (newMovement.vecY < 0.f) newMovement.vecY = 0.f;									//�O���E���h�������ɂ͂����Ȃ�

	//�Փˌ�̈ʒu�ƈړ��x�N�g����ݒ�
	const float	MAX_LENGTH			= DirectX::XMVector3Length(movement_).vecX;											//�t���[���̈ړ�����
	const float	SURPLUS_DISTANCE	= DirectX::XMVector3Length(transform_.position_ - COLLISION_POSITION_BALL).vecX;	//�Փˌ�̗]�苗��
	const float	PERCENTAGE			= SURPLUS_DISTANCE / MAX_LENGTH;													//�Փ˃t���[���̏ՓˑO��̈ړ��䗦
	transform_.position_			= COLLISION_POSITION_BALL + (newMovement * PERCENTAGE);								//�Փ˃t���[���̈ʒu
	movement_						= newMovement;																		//�Փˌ�̈ړ��x�N�g��

	//�Փˌ�̉�]��ݒ�
	rotationAxis_	= GetRotateAxis(NORMAL_DIRECTION);							//��]��
	rotation_		= GetRotationSpeedAfterCollision(NORMAL_DIRECTION);			//��]��
}

//�͂̋z���x�N�g���i�e���̋t�x�N�g���j�̎擾
XMVECTOR Ball::GetAbsorption(const XMVECTOR& normal, const std::string& targetName) const
{
	const XMVECTOR	EFFECT_DIRECTION		= DirectX::XMVector3Normalize(-normal);								//�Փ˖ʂ̖@���x�N�g��
	
	const float		DOT_EFFECT_MOVEMENT		= DirectX::XMVector3Dot(EFFECT_DIRECTION, movement_).vecX;			//���ʂ�������������x�N�g���Ɖ^���x�N�g���̓���

	const float		RESTITUTION_BALL		= ExternalDataManager::GetCoefficientRestitution(GetName());		//�{�[���̔����W��
	const float		RESTITUTION_TARGET		= ExternalDataManager::GetCoefficientRestitution(targetName);		//�Փ˂����I�u�W�F�N�g�̔����W��
	const float		COEFFICIENT				= 1.f - RESTITUTION_BALL * RESTITUTION_TARGET;						//�����W��

	return EFFECT_DIRECTION * DOT_EFFECT_MOVEMENT * COEFFICIENT;												//�͂̋z���x�N�g��
}

//���C�͂̎擾
XMVECTOR Ball::GetFriction(const XMVECTOR& normal, const std::string& targetName, float* friction) const
{ 
	using DirectX::XMVector3Normalize;
	using DirectX::XMVector3Cross;
	using DirectX::XMVector3Length;

	const XMVECTOR	MOVEMENT_DIRECTION				= XMVector3Normalize(movement_);												//�ړ��x�N�g���̌����x�N�g��
	const XMVECTOR	NORMAL_DIRECTION				= XMVector3Normalize(normal);													//�Փ˖ʂ̖@���x�N�g��
	const XMVECTOR	ROTATINOAL_FRICTION_DIRECTION	= XMVector3Normalize(XMVector3Cross(NORMAL_DIRECTION, rotationAxis_));			//��]�ɂ�門�C�͂���������
	const XMVECTOR	CROSS_NORMAL_MOVEMENT			= XMVector3Cross(NORMAL_DIRECTION, MOVEMENT_DIRECTION);							//�ړ��x�N�g���ƏՓ˖ʖ@���̊O��
	const XMVECTOR	MOVEMENT_FRICTION_DIRECTION		= XMVector3Normalize(XMVector3Cross(NORMAL_DIRECTION, CROSS_NORMAL_MOVEMENT));	//�^���ɂ�門�C�͂���������
	const float		BALL_RADIUS						= ExternalDataManager::GetBallRadius();											//�{�[���̔��a(m)
	const float		BALL_WEIGHT						= ExternalDataManager::GetBallWeight();											//�{�[���̏d��(kg)
	const float		DEGREE_MAX						= 360.f;																		//�ő�p�x(deg)
	const float		DOT								= DirectX::XMVector3Dot(NORMAL_DIRECTION, -MOVEMENT_DIRECTION).vecX;			//�ړ��x�N�g���ƏՓ˖ʂ̖@���x�N�g���̓���
	const float		VELOCITY						= XMVector3Length(movement_).vecX;												//�{�[���̑��x(m/f)
	const float		COEFFICIENT_BALL				= ExternalDataManager::GetCoefficientFriction(GetName());						//�{�[���̓����C�W��
	const float		COEFFICIENT_TERGET				= ExternalDataManager::GetCoefficientFriction(targetName);						//�Փ˂����I�u�W�F�N�g�̓����C�W��
	const float		COEFFICIENT						= COEFFICIENT_BALL * COEFFICIENT_TERGET;										//�����C�W��
	const float		ANGULAR_VELOCITY				= 2.f * DirectX::XM_PI * rotation_;												//�p���x(degree/f)
	const float		NORMAL_FORCE					= BALL_WEIGHT * (1.f + VELOCITY * DOT);											//�����R��
	const float		FRICTION_FORCE					= COEFFICIENT * NORMAL_FORCE;													//�����C��
	const float		INERTIA							= NORMAL_FORCE * powf(BALL_RADIUS, 2);											//�������[�����g
	const float		RORTATINOAL_FORCE				= 0.5f * INERTIA *  powf(ANGULAR_VELOCITY, 2);									//��]�ɂ��^����

	const XMVECTOR	MOVEMENT_FRICTION				= MOVEMENT_FRICTION_DIRECTION * FRICTION_FORCE;									//�ړ������Ɋւ��ē�����
	const XMVECTOR	RORTATINOAL_FRICTION			= ROTATINOAL_FRICTION_DIRECTION * RORTATINOAL_FORCE * FRICTION_FORCE;			//��]�����Ɋւ��ē�����

	if (friction != nullptr) *friction = FRICTION_FORCE;																			//���C�͂��o�͕ϐ��ɓ���
	
	return MOVEMENT_FRICTION + RORTATINOAL_FRICTION;																				//���C��
}

//��]���̎擾
float Ball::GetRotationSpeedAfterCollision(const XMVECTOR& normal) const
{
	const XMVECTOR	MOVEMENT_DIRECTION	= DirectX::XMVector3Normalize(movement_);			//�ړ�����
	const XMVECTOR	COLLISION_DIRECTION = DirectX::XMVector3Normalize(normal);				//�Փ˖ʂ̖@���x�N�g��
	const float		BALL_RADIUS			= ExternalDataManager::GetBallRadius();				//�{�[���̔��a(m)
	const float		CIRCUMFERENCE		= 2.f * DirectX::XM_PI * BALL_RADIUS;				//�~��(m)
	const float		VELOCITY			= DirectX::XMVector3Length(movement_).vecX;			//���x(m/f)
	
	return VELOCITY / CIRCUMFERENCE;														//��]��(rotate/f)
}

//��]���̎擾
XMVECTOR Ball::GetRotateAxis(const XMVECTOR& normal) const
{
	return DirectX::XMVector3Normalize(DirectX::XMVector3Cross(movement_, normal));		//��]������
}