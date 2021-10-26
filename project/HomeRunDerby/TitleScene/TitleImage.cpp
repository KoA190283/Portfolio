#include "TitleImage.h"

#include "../Engine/Image.h"
#include "../ExternalDataManager.h"

TitleImage::TitleImage()
{
}

//������
void TitleImage::Initialize()
{
	//�摜�̓ǂݍ���
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathTitleSceneImage(ExternalDataManager::TITLE_SCENE_IMAGE::TITLE);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																			//�摜�̃n���h���ԍ�
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//�`��ʒu�̐ݒ�
	transform_.position_ = DirectX::XMVectorSet(0, 0.4f, 0, 0);		
}

//�X�V
void TitleImage::Update()
{
}

//�`��
void TitleImage::Draw()
{
	//�`��
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathTitleSceneImage(ExternalDataManager::TITLE_SCENE_IMAGE::TITLE);	//�^�C�g���摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																		//�^�C�g���摜�̃n���h���ԍ�
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);
}

//���
void TitleImage::Release()
{
}