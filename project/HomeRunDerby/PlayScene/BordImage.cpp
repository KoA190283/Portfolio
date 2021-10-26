#include "BordImage.h"

#include "../Engine/Image.h"
#include "../ExternalDataManager.h"

BordImage::BordImage() : BordImage(nullptr)
{
}

BordImage::BordImage(PlayScene* pPlayScene)
{
	pPlayScene_ = pPlayScene;
}

//������
void BordImage::Initialize()
{
	//�摜�̓ǂݍ���
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathPlaySceneImage(ExternalDataManager::PLAY_SCENE_IMAGE::SCORE_BOARD);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																				//�摜�̃n���h���ԍ�
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//�`��ʒu�̐ݒ�
	transform_.position_	= DirectX::XMVectorSet(0.55f, -0.55f, 0.0001f, 0);	//�`��ʒu
}

//�X�V
void BordImage::Update()
{
}

//�`��
void BordImage::Draw()
{
	if (pPlayScene_ == nullptr)		return;	//�v���C�V�[���̃|�C���^�����݂��Ȃ���Ε`�悵�Ȃ�
	if (pPlayScene_->IsInPlay())	return;	//�v���[���ł���Ε`�悵�Ȃ�

	//�`��
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathPlaySceneImage(ExternalDataManager::PLAY_SCENE_IMAGE::SCORE_BOARD);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																			//�摜�̃n���h���ԍ�
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);
}

//���
void BordImage::Release()
{
}