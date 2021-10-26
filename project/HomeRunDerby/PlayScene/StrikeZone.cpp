#include "../Engine/BoxCollider.h"
#include "../Engine/Camera.h"
#include "../Engine/Input.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../ExternalDataManager.h"
#include "Ball.h"
#include "Batter.h"
#include "Pitcher.h"
#include "PlayScene.h"
#include "StrikeZone.h"

using DirectX::XMVECTOR;
using DirectX::XMMATRIX;

//�R���X�g���N�^
StrikeZone::StrikeZone(IGameObject* parent)
	: IGameObject(parent, "StrikeZone")
{
}

//������
void StrikeZone::Initialize()
{
	//transform_�̐ݒ�
	transform_.position_	= ExternalDataManager::GetStrikeZoneCenter();	//���S�ʒu
	transform_.scale_.vecX	= ExternalDataManager::GetStrikeZoneWidth();	//����
	transform_.scale_.vecY	= ExternalDataManager::GetStrikeZoneHeight();	//����
	transform_.Calclation();

	//���f���f�[�^�̃��[�h
	const int MODEL_HANDLE = Model::Load(ExternalDataManager::GetFilePathStrikeZoneModel());
	assert(MODEL_HANDLE != Model::ERROR_CORD);

	//�����蔻��̐ݒ�ǉ�
	const XMVECTOR	COLLIDER_RELATIVE_POSITION	= DirectX::g_XMZero;												//�I�u�W�F�N�g���瓖���蔻��ւ̃x�N�g��
	BoxCollider*	newCollider					= new BoxCollider(&COLLIDER_RELATIVE_POSITION, transform_.scale_);	//�V���������蔻��
	AddCollider(newCollider);
}

//�X�V
void StrikeZone::Update()
{
	//�}�E�X�̍��{�^���������ꂽ�牟���ꂽ���̏������Ă�
	if (Input::IsMouseButtonDown(0))	Pressed();
}

//�`��
void StrikeZone::Draw()
{
	//�`�悷�郂�f���̏�Ԃ�ݒ肵�ĕ`��
	const int MODEL_HANDLE = Model::GetHandle(ExternalDataManager::GetFilePathStrikeZoneModel());	//�X�g���C�N�]�[���̃��f���n���h��
	Model::SetTransform(MODEL_HANDLE, transform_);
	Model::Draw(MODEL_HANDLE, Direct3D::SHADER_TYPE::NO_SHADE_3D);
	
	//�I�ɂȂ�{�[���̕`��
	DrawTagetBall();
}

//���
void StrikeZone::Release()
{
}

//���I�u�W�F�N�g�ƏՓ˂������̏���
void StrikeZone::OnCollision(IGameObject* pTarget, CollisionData* pData)
{
	//����̍X�V���L���łȂ���Ή������Ȃ�
	if (!pTarget->IsActive())	return;

	//�X�V���~�߂�
	Inactivate();

	//�`����~�߂�
	Invisible();
}

//�����ꂽ�i�N���b�N���ꂽ�j�Ƃ��̏���
void StrikeZone::Pressed()
{
	//�}�E�X�̈ʒu���X�g���C�N�]�[���Əd�Ȃ��Ă��邩���m�F����
	//�d�Ȃ��Ă�����d�Ȃ����ʒu�Ɍ������ăo�b�^�[���X�C���O������

	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::operator*;

	//�r���[�|�[�g�s��̍쐬
	float w = Global::WINDOW_WIDTH / 2.0f;	//�E�B���h�E��
	float h = Global::WINDOW_HEIGHT / 2.0f;	//�E�B���h�E����

	const XMMATRIX VIEW_PORT = {			//�r���[�|�[�g�s��
		w, 0, 0, 0,
		0,-h, 0, 0,
		0, 0, 1, 0,
		w, h, 0, 1
	};

	//�}�E�X�̈ʒu�����[���h���W�ɕϊ����邽�߂̍s�������
	XMMATRIX inverseMatrix;																				//�}�E�X�̈ʒu�����W�ɕϊ����邽�߂̍s��
	inverseMatrix =  XMMatrixInverse(nullptr, VIEW_PORT);												//�r���[�|�[�g�s��
	inverseMatrix *= XMMatrixInverse(nullptr, Camera::GetProjectionMatrix());							//�v���W�F�N�V�����s��
	inverseMatrix *= XMMatrixInverse(nullptr, Camera::GetViewMatrix());									//�r���[�s��

	//�}�E�X�̃��[���h���W���쐬
	XMVECTOR mousePositionFront, mousePositionBack;														//�}�E�X�̍��W
	mousePositionFront		= Input::GetMousePosition();												//�}�E�X�̍��W�i��O�j
	mousePositionBack		= Input::GetMousePosition();												//�}�E�X�̍��W�i���j
	mousePositionFront.vecZ = 0.0f;																		//�����̈ړ����O�i��O�j��
	mousePositionBack.vecZ	= 1.0f;																		//�����̈ړ����P�i���j��

	mousePositionFront	= XMVector3TransformCoord(mousePositionFront, inverseMatrix);					//�ϊ��s��Ŏ�O�̃}�E�X���W��ϊ�
	mousePositionBack	= XMVector3TransformCoord(mousePositionBack, inverseMatrix);					//�ϊ��s��ŉ��̃}�E�X���W��ϊ�

	//���C�L���X�g�Ɏg�p����\���̂̏���	
	RayCastData data;																					//���C�L���X�g�Ŏg�p����f�[�^
	data.start		= mousePositionFront;																//���C�̊J�n�ʒu
	data.direction	= DirectX::XMVector3Normalize(mousePositionBack - mousePositionFront);				//���C�̕���
	
	//���C�L���X�g
	const int MODEL_HANDLE = Model::GetHandle(ExternalDataManager::GetFilePathStrikeZoneModel());		//�X�g���C�N�]�[���̃��f���n���h��
	Model::SetTransform(MODEL_HANDLE, transform_);														//���肷��I�u�W�F�N�g��transform_����ݒ�
	Model::RayCast(MODEL_HANDLE, &data);																//���C�L���X�g

	//���C���������Ă�����o�b�^�[�̃X�C���O�֐����Ă�
	if (data.isHit)
	{
		//�N���b�N���ꂽ�ʒu���擾����
		XMVECTOR clickPosistion = data.direction * data.distance + data.start;							//�N���b�N���ꂽ�ʒu
		clickPosistion			= XMVector3TransformCoord(clickPosistion, transform_.GetWorldMatrix());	//���[���h�s��ŕϊ�
		clickPosistion.vecZ		= 0;																	//Z�����̂�����Ȃ���

		//�o�b�^�[�̃X�C���O�֐����Ă�
		Batter*	 pBatter = (Batter*)(GetParent()->FindChildObject("Batter"));							//�o�b�^�[�̃|�C���^
		pBatter->Swing(clickPosistion);																	//�X�C���O�֐��̌Ăяo��
	}
}

//�I�ɂȂ�{�[���̕`��
void StrikeZone::DrawTagetBall()
{
	using DirectX::XMVector3Normalize;
	using DirectX::XMVector3Cross;
	using DirectX::operator-=;
	using DirectX::operator*=;
	using DirectX::operator+;
	using DirectX::operator-;
	using DirectX::operator*;

	//�v���[���J�n����Ă��Ȃ���Ε`�悵�Ȃ�
	PlayScene*	pPlayScene	= (PlayScene*)pParent_;								//�v���C�V�[���̃|�C���^
	if (!pPlayScene->IsInPlay())	return;

	//�{�[���̍X�V���L���łȂ���Ή������Ȃ�
	Ball*		pBall		= (Ball*)(pParent_->FindChildObject("Ball"));		//�o�b�^�[�̃|�C���^
	if (!pBall->IsActive())	return;

	Pitcher*	pPitcher	= (Pitcher*)(pParent_->FindChildObject("Pitcher"));	//�s�b�`���[�̃|�C���^
	
	//�I�p�{�[����`�悷��ʒu�����߂�
	const XMVECTOR	STRIKE_ZONE_CENTER_POSITION	= ExternalDataManager::GetStrikeZoneCenter();														//�X�g���C�N�]�[���̒��S�ʒu
	XMVECTOR		drawPosition				= STRIKE_ZONE_CENTER_POSITION + pPitcher->GetAimPosition();											//�`��ʒu

	//�I������Ă��鋅�킪�X�g���[�g�ȊO�Ȃ�X�g���C�N�]�[����̑_���Ă���ꏊ����ω������̋t�����ɕ`��ʒu�𓮂���	
	const int		BALL_TYPE					= pPitcher->GetBallType();																			//�I������Ă��鋅��
	const int		FAST_BALL					= 0;																								//�t�@�X�g�{�[���i�X�g���[�g�j
	if (BALL_TYPE != FAST_BALL)
	{
		//�`���̃{�[���̕ω���ݒ肷��
		const float		DEGREE_MAX					= 360.f;																												//�ő�p�x
		const float		CURVE_ADJUSTMENT			= 0.0000002f;																											//�ω��ʂ𒲐����邽�߂̒l
		const float		ROTATE						= ExternalDataManager::GetRotate(BALL_TYPE) * DEGREE_MAX;																//��]��
		const XMVECTOR	ROTATE_AXIS					= ExternalDataManager::GetRotateAxis(BALL_TYPE);																		//��]��
		const XMVECTOR	PITCHER_TO_HOME_DIRECTION	= XMVector3Normalize(ExternalDataManager::GetHomePlatePosition() - ExternalDataManager::GetPitcherPlatePosition());		//�s�b�`���[����z�[���ւ̒P�ʃx�N�g��
		const XMVECTOR	CURVE_DIRECTION				= XMVector3Normalize(XMVector3Cross(ROTATE_AXIS, PITCHER_TO_HOME_DIRECTION));											//�Ȃ�������ւ̒P�ʃx�N�g��
		const XMVECTOR	DRAW_CURVE_MAX				= CURVE_DIRECTION * ROTATE * CURVE_ADJUSTMENT;																			//�`���̍ő�ω��x�N�g��
		const float		TO_BALL_DISTANCE			= (pBall->GetPosition() - STRIKE_ZONE_CENTER_POSITION).vecZ;															//�X�g���C�N�]�[���ƃ{�[����Z�����̋���
		const float		TO_PITCHER_DISTANCE			= (pPitcher->GetReleasePosition() - STRIKE_ZONE_CENTER_POSITION).vecZ;													//�X�g���C�N�]�[���ƃ����[�X�|�C���g��Z�����̋���
		const float		CURVE_PERCENTAGE			= TO_BALL_DISTANCE / TO_PITCHER_DISTANCE;																				//�ω��ʂ̊���

		//�ω������Ƌt�����Ɋ��������s�b�`���[���_���Ă���ꏊ���炸�炷
		drawPosition -= DRAW_CURVE_MAX * CURVE_PERCENTAGE;											
	}

	//�I�p�{�[���̉�]��Ԃ����߂�
	static float	rotation			= 0.f;																						//��]��
	const XMVECTOR	ROTATE_AXIS			= ExternalDataManager::GetRotateAxis(BALL_TYPE);											//��]��
	const XMVECTOR  QUATERNION			= DirectX::XMQuaternionRotationAxis(ROTATE_AXIS, DirectX::XMConvertToRadians(rotation));	//��]���
	rotation += 40.f;																												//1f���̉�]�ʂ����Z����

	//�I�p�{�[����`�悷���Ԃ��쐬����
	Transform transform;
	transform.position_		=	drawPosition;							//�`��ʒu
	transform.scale_		*=	ExternalDataManager::GetBallRadius();	//�g�嗦
	transform.quaternion_	=	QUATERNION;								//��]���

	//�쐬���������g�p���ĕ`�悷��
	const int MODEL_HANDLE	=	Model::GetHandle(ExternalDataManager::GetFilePathBallModel());	//�{�[���̃��f���n���h��
	Model::SetTransform(MODEL_HANDLE, transform);
	Model::Draw(MODEL_HANDLE, Direct3D::SHADER_TYPE::NO_SHADE_3D);
}
