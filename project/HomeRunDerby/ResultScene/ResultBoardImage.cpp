#include "ResultBoardImage.h"

#include "../Engine/Image.h"
#include "../ExternalDataManager.h"

ResultBoardImage::ResultBoardImage()
{
}

//������
void ResultBoardImage::Initialize()
{
	//�摜�̓ǂݍ���
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathResultSceneImage(ExternalDataManager::RESULT_SCENE_IMAGE::SCORE_BOARD);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																					//�摜�̃n���h���ԍ�
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//�`��ʒu�̐ݒ�
	transform_.position_	= DirectX::XMVectorSet(0.f,  0.4f, 0.0001f, 0);	//�`��ʒu
	transform_.scale_		= DirectX::XMVectorSet(2.0f, 1.3f, 0, 0);			//�g��T�C�Y
}

//�X�V
void ResultBoardImage::Update()
{
}

//�`��
void ResultBoardImage::Draw()
{
	//�`��
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathResultSceneImage(ExternalDataManager::RESULT_SCENE_IMAGE::SCORE_BOARD);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																				//�摜�̃n���h���ԍ�
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);
}

//���
void ResultBoardImage::Release()
{
}