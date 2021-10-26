#include "PurposeImage.h"

#include "../Engine/Image.h"
#include "../ExternalDataManager.h"

PurposeImage::PurposeImage() : PurposeImage(nullptr)
{
}

PurposeImage::PurposeImage(PlayScene* pPlayScene)
{
	pPlayScene_ = pPlayScene;
}

//������
void PurposeImage::Initialize()
{
	//�����̉摜�f�[�^�̓ǂݍ���
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathPlaySceneImage(ExternalDataManager::PLAY_SCENE_IMAGE::PURPOSE);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																			//�摜�̃n���h���ԍ�
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//�`��ʒu�̐ݒ�
	transform_.position_	= DirectX::XMVectorSet(0.55f, 0.75f, 0, 0);	//�`��ʒu
	transform_.scale_		= DirectX::XMVectorSet(0.5f, 0.5f, 0, 0);	//�g��T�C�Y
}

//�X�V
void PurposeImage::Update()
{
}

//�`��
void PurposeImage::Draw()
{
	if (pPlayScene_ == nullptr)		return;	//�v���C�V�[���̃|�C���^�����݂��Ȃ���Ε`�悵�Ȃ�
	if (pPlayScene_->IsInPlay())	return;	//�v���[���ł���Ε`�悵�Ȃ�

	//�`��
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathPlaySceneImage(ExternalDataManager::PLAY_SCENE_IMAGE::PURPOSE);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																		//�摜�̃n���h���ԍ�
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);
}

//���
void PurposeImage::Release()
{
}