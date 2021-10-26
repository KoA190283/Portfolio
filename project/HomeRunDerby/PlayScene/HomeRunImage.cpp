#include "HomeRunImage.h"

#include "../Engine/Image.h"
#include "../ExternalDataManager.h"

HomeRunImage::HomeRunImage() : HomeRunImage(nullptr)
{
}

HomeRunImage::HomeRunImage(PlayScene* pPlayScene)
{
	pPlayScene_ = pPlayScene;
}

//������
void HomeRunImage::Initialize()
{
	//�摜�̓ǂݍ���
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathPlaySceneImage(ExternalDataManager::PLAY_SCENE_IMAGE::HOME_RUN);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																			//�摜�̃n���h���ԍ�
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//�`��ʒu�̐ݒ�
	transform_.position_	= DirectX::XMVectorSet(0, 0.2f, 0, 0);			//�`��ʒu
	transform_.scale_		= DirectX::XMVectorSet(1.5f, 1.5f, 0, 0);		//�g��T�C�Y
}

//�X�V
void HomeRunImage::Update()
{
}

//�`��
void HomeRunImage::Draw()
{	
	if (pPlayScene_ == nullptr)		return;							//�v���C�V�[���̃|�C���^�����݂��Ȃ���Ε`�悵�Ȃ�
	if (!pPlayScene_->IsInPlay())	return;							//�v���[���łȂ���Ε`�悵�Ȃ�
	if (!pPlayScene_->IsHomeRun())	return;							//�z�[�������łȂ���Ε`�悵�Ȃ�

	//�����摜�̕`��
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathPlaySceneImage(ExternalDataManager::PLAY_SCENE_IMAGE::HOME_RUN);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																		//�摜�̃n���h���ԍ�
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);
}

//���
void HomeRunImage::Release()
{
}