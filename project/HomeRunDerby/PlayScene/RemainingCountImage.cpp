#include "RemainingCountImage.h"

#include "../Engine/Image.h"
#include "../ExternalDataManager.h"
#include "../InternalPlayDataReservoir.h"

RemainingCountImage::RemainingCountImage() : RemainingCountImage(nullptr)
{
}

RemainingCountImage::RemainingCountImage(PlayScene* pPlayScene)
{
	pPlayScene_ = pPlayScene;
}

//������
void RemainingCountImage::Initialize()
{
	//�����̉摜�f�[�^�̓ǂݍ���
	for (int i = 0; i < ExternalDataManager::DECIMAL_BASE; i++)
	{
		//����i�̉摜�f�[�^�̓ǂݍ���
		const int IMAGE_HANDLE = Image::Load(ExternalDataManager::GetFilePathNumberImage(i));
		assert(IMAGE_HANDLE != Image::ERROR_CORD);
	}

	//�����̉摜�f�[�^�̓ǂݍ���
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathPlaySceneImage(ExternalDataManager::PLAY_SCENE_IMAGE::REMAINING);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																			//�摜�̃n���h���ԍ�
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//�`��ʒu�̐ݒ�
	transform_.position_	= DirectX::XMVectorSet(0.45f, -0.42f, 0, 0);	//�`��ʒu
	transform_.scale_		= DirectX::XMVectorSet(0.5f, 0.5f, 0, 0);		//�g��T�C�Y
}

//�X�V
void RemainingCountImage::Update()
{
}

//�`��
void RemainingCountImage::Draw()
{
	if (pPlayScene_ == nullptr)		return;	//�v���C�V�[���̃|�C���^�����݂��Ȃ���Ε`�悵�Ȃ�
	if (pPlayScene_->IsInPlay())	return;	//�v���[���ł���Ε`�悵�Ȃ�

	//���_������摜�`��
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathPlaySceneImage(ExternalDataManager::PLAY_SCENE_IMAGE::REMAINING);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																		//�摜�̃n���h���ԍ�
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);

	//�����`��p�摜�̃n���h���ԍ��z��쐬
	int numberImageHandles[ExternalDataManager::DECIMAL_BASE];													//�����`��p�摜�̃n���h���ԍ��z��
	for (int i = 0; i < ExternalDataManager::DECIMAL_BASE; i++)
	{
		numberImageHandles[i] = Image::GetHandle(ExternalDataManager::GetFilePathNumberImage(i));
	}

	//�`��ʒu�𕶎��񕪂��炷
	const float SCORE_IMAGE_WIDE = 0.38f;
	transform_.position_.vecX	+= SCORE_IMAGE_WIDE;

	//�c��񐔂̕`��
	const float INTERVAL_WIDE = 0.07f;																			//1�������Ƃ̊Ԋu
	Image::DrawNaturalNumber(numberImageHandles, InternalPlayDataReservoir::GetRemainingCount(), transform_, INTERVAL_WIDE);

	//�`��ʒu��߂�
	transform_.position_.vecX -= SCORE_IMAGE_WIDE;
}

//���
void RemainingCountImage::Release()
{
}