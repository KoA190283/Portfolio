#include "../Engine/Global.h"
#include "../Engine/SceneManager.h"
#include "../Engine/Input.h"
#include "../Engine/Camera.h"
#include "../BackGround.h"
#include "../ExternalDataManager.h"
#include "../InternalPlayDataReservoir.h"
#include "Ball.h"
#include "Batter.h"
#include "BordImage.h"
#include "Field.h"
#include "HomeRunImage.h"
#include "Pitcher.h"
#include "PlayScene.h"
#include "PurposeImage.h"
#include "RemainingCountImage.h"
#include "ScoreImage.h"
#include "StrikeZone.h"

using XMVECTOR = DirectX::XMVECTOR;

//�R���X�g���N�^
PlayScene::PlayScene(IGameObject* parent)
	: IGameObject(parent, "PlayScene"), pUmpire_(nullptr), timer_(0)
{
}

//������
void PlayScene::Initialize()
{
	//�v���C�V�[���Ŏg�p����f�[�^�̏�����
	ExternalDataManager::InitializePlaySceneData();
	InternalPlayDataReservoir::InitializePlayData();

	//�R���̏�����
	pUmpire_ = new Umpire;

	//�w�i�̏�����
	BackGround::Initialize();

	//�v���C�V�[�����̃I�u�W�F�N�g�̃C���X�^���X��
	Instantiate<Field>(this);
	Instantiate<Ball>(this);
	Instantiate<Batter>(this);
	Instantiate<Pitcher>(this);
	Instantiate<StrikeZone>(this);

	//UI�̃C���X�^���X�𐶐������X�g�ɒǉ�
	userInterfaceList_.push_back(new BordImage(this));
	userInterfaceList_.push_back(new HomeRunImage(this));
	userInterfaceList_.push_back(new PurposeImage(this));
	userInterfaceList_.push_back(new RemainingCountImage(this));
	userInterfaceList_.push_back(new ScoreImage(this));

	//UI�̏�����
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Initialize();
	}

	//�J�����������ʒu�ɐݒ肷��
	ResetCamera();
}

//�X�V
void PlayScene::Update()
{
	using DirectX::operator-;

	//�Q�[���̏I���t���O��true�Ȃ烊�U���g�V�[���ɐ؂�ւ���
	if (pUmpire_->IsEndGame())
	{
		SceneManager* sceneManager = (SceneManager*)pParent_;
		sceneManager->ChangeScene(SCENE_ID::RESULT);
		return;
	}

	//UI�̍X�V
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Update();
	}
	
	//�i�s�󋵂��X�V
	UpdateProgress();

	//�{�[�����o�b�g�ɓ������Ă�����J�����̓{�[����ǂ�
	if (pUmpire_->IsHitBall())
	{
		//�{�[���̈ʒu���擾
		Ball* pBall = (Ball*)FindChildObject("Ball");
		const XMVECTOR BALL_POSITION = pBall->GetPosition();
		
		//�򋗗������ȏ゠������J�����̈ʒu��ύX����
		const float DISTANCE_LIMIT			= 50.f;
		const float DISTENCE_HOME_TO_BALL	= DirectX::XMVector3Length(BALL_POSITION - ExternalDataManager::GetHomePlatePosition()).vecX;
		if (DISTENCE_HOME_TO_BALL > DISTANCE_LIMIT)
		{
			XMVECTOR newCameraPosition;
			newCameraPosition.vecX = 0.f;		
			newCameraPosition.vecY = 20.f;			
			newCameraPosition.vecZ = -70.f;			

			Camera::SetPosition(newCameraPosition);
		}

		//�J�����̃^�[�Q�b�g���{�[���̌��݂̈ʒu�ɕύX
		Camera::SetTarget(BALL_POSITION);
	}
}

//�`��
void PlayScene::Draw()
{
	//�w�i�`��
	BackGround::Draw();
}

//���
void PlayScene::Release()
{
	SAFE_DELETE(pUmpire_);
	
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		SAFE_RELEASE(pUserInterface);
	}
	userInterfaceList_.clear();

	ExternalDataManager::Release();
}

//�q���̕`����s������ɍs���`�揈��
void PlayScene::DrawAfterChildren()
{	
	//UI�̕`��
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Draw();
	}
}

//�{�[���������ƏՓ˂����Ƃ��̏���
void PlayScene::CollisionBall(const XMVECTOR & collisionPosision, const std::string & objectName)
{
	if (!pUmpire_->IsInPlay())	return;	//�v���[���łȂ���Ή������Ȃ�

	//�{�[���Ɠ��������I�u�W�F�N�g���o�b�^�[�������ꍇ�̏���
	if (objectName == "Batter")
	{
		//�X�g���C�N�]�[���̍X�V�ƕ`��𖳌�������
		StrikeZone* pStrikeZone = (StrikeZone*)FindChildObject("StrikeZone");
		pStrikeZone->Inactivate();
		pStrikeZone->Invisible();
	}
	
	//�Փ˂����I�u�W�F�N�g�����t�B�[���h�Ńt���C�̏�Ԃł���΃^�C�}�[��0�ɂ���
	if (objectName == "Field")
		if (!pUmpire_->IsGroundBall())	timer_ = 0;

	//�{�[���������ƏՓ˂����Ƃ��̏������s��
	pUmpire_->CollisionBall(collisionPosision, objectName);
}

//�Q�[�����̑��x���擾
float PlayScene::GetGameSpeedAcceleration() const
{
	//�o�b�g�ƃ{�[�����������Ă����猻����菭����������
	if (pUmpire_->IsHitBall())	return 2.0f;

	//�������Ă��Ȃ���Β������ꂽ���x
	else						return ExternalDataManager::GetGameSpeedAcceleration();
}

//�v���[���ł��邩���擾
bool PlayScene::IsInPlay() const
{
	return pUmpire_->IsInPlay();
}

//�ŋ������擾
bool PlayScene::IsHitBall() const
{
	return pUmpire_->IsHitBall();
}

//�z�[�����������擾
bool PlayScene::IsHomeRun() const
{
	return pUmpire_->IsHomeRun();
}

//�i�s�󋵂��X�V
void PlayScene::UpdateProgress()
{
	timer_++;

	//�v���[���̏���
	if (pUmpire_->IsInPlay())
	{
		//�n�ʂɃ{�[�������Ă���ꍇ�̏���
		if (pUmpire_->IsGroundBall())
		{
			//�t�@�E���{�[���Ȃ�v���[���I������
			if (pUmpire_->IsFoulBall())	EndPlay();

			//��莞�Ԃ��o�߂��Ă���΃v���[���I������
			const int END_PLAY_TIME = 120;			//�n�ʂɃ{�[�������Ă���v���[���I������܂ł̎���
			if (timer_ > END_PLAY_TIME)	EndPlay();
		}
	}
	//�v���[���ł͂Ȃ��ꍇ�̏���
	else
	{
		//�J�n���ԂɂȂ�����v���[���J�n����
		const int BEGIN_PLAY_TIME = 120;		//�v���[���Ă��Ȃ�����
		if (timer_ > BEGIN_PLAY_TIME)	BeginPlay();
	}
}

//�v���[�J�n
void PlayScene::BeginPlay()
{
	//�s�b�`���[�̓����̏���������
	Pitcher* pPitcher = (Pitcher*)FindChildObject("Pitcher");
	pPitcher->InitializePitch();

	//�{�[���̕`���L����
	Ball* pBall = (Ball*)FindChildObject("Ball");
	pBall->Visible();
	pBall->Inactivate();

	//�v���[���J�n����
	pUmpire_->BeginPlay();
}

//�v���[�I��
void PlayScene::EndPlay()
{
	//�v���[���I������
	pUmpire_->EndPlay();

	//�c��̋�����0�Ȃ�Q�[���̏I���t���O��true�ɂ���
	if (InternalPlayDataReservoir::GetRemainingCount() == 0)
	{
		pUmpire_->EndGame();
		return;
	}

	//�{�[���̍X�V�ƕ`��𖳌���
	Ball* pBall = (Ball*)FindChildObject("Ball");
	pBall->Inactivate();
	pBall->Invisible();

	//�o�b�^�[�̃A�j���[�V�������ŏ��̏�Ԃɖ߂�
	Batter* pBatter = (Batter*)FindChildObject("Batter");
	pBatter->ResetAnimation();

	//�s�b�`���[�̃A�j���[�V�������ŏ��̏�Ԃɖ߂�
	Pitcher* pPitcher = (Pitcher*)FindChildObject("Pitcher");
	pPitcher->ResetAnimation();

	//�X�g���C�N�]�[���̍X�V�ƕ`���L��������
	StrikeZone* pStrikeZone = (StrikeZone*)FindChildObject("StrikeZone");
	pStrikeZone->Activate();
	pStrikeZone->Visible();

	//�J�����������ʒu�ɖ߂�
	ResetCamera();

	//�^�C�}�[��0�ɂ���
	timer_ = 0;
}

//�J�������ŏ��̈ʒu�ɖ߂�
void PlayScene::ResetCamera() const
{
	Camera::SetPosition(0.f, 1.2f, 4.f);	//�ʒu
	Camera::SetTarget(0, 1.f, -1.f);		//����ʒu
}
