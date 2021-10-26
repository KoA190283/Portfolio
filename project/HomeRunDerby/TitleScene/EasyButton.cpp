#include "EasyButton.h"

#include "../Engine/Image.h"
#include "../Engine/Input.h"
#include "../Engine/Global.h"
#include "../ExternalDataManager.h"
#include "../InternalPlayDataReservoir.h"

EasyButton::EasyButton() :
	EasyButton(nullptr)
{
}

EasyButton::EasyButton(TitleScene* pTitleScene)
{
	pTitleScene_ = pTitleScene;
}

//������
void EasyButton::Initialize()
{
	//�摜�̓ǂݍ���
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathTitleSceneImage(ExternalDataManager::TITLE_SCENE_IMAGE::MODE_EASY);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																				//�摜�̃n���h���ԍ�
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//�`��ʒu�̐ݒ�
	transform_.position_	= DirectX::XMVectorSet(-0.6f, -0.4f, 0, 0);
	transform_.scale_		= DirectX::XMVectorSet(0.85f, 0.85f, 0, 0);
}

//�X�V
void EasyButton::Update()
{
	if (pTitleScene_ == nullptr) return;	//�^�C�g���V�[���̃|�C���^��������΍X�V�������s��Ȃ�

	//�N���b�N����Ă��邩���m�F���A����Ă��Ȃ���΍X�V�������s��Ȃ�
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathTitleSceneImage(ExternalDataManager::TITLE_SCENE_IMAGE::MODE_EASY);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																			//�摜�̃n���h���ԍ�
	Image::SetTransform(IMAGE_HANDLE, transform_);
	if (!Image::IsClicked(IMAGE_HANDLE))	return;

	//�Q�[�����[�h�̐ݒ�
	InternalPlayDataReservoir::SetGameMode(ExternalDataManager::GAME_MODE::EASY);

	//���̃V�[���ɕύX
	pTitleScene_->ChangeNextScene();
}

//�`��
void EasyButton::Draw()
{
	//�`��
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathTitleSceneImage(ExternalDataManager::TITLE_SCENE_IMAGE::MODE_EASY);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																			//�摜�̃n���h���ԍ�
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);
}

//���
void EasyButton::Release()
{
}
