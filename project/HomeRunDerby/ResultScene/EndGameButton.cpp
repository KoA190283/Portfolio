#include "EndGameButton.h"

#include "../Engine/Image.h"
#include "../ExternalDataManager.h"

EndGameButton::EndGameButton() :
	EndGameButton(nullptr)
{
}

EndGameButton::EndGameButton(ResultScene* pResultScene)
{
	pResultScene_ = pResultScene;
}

//������
void EndGameButton::Initialize()
{
	//�����̉摜�f�[�^�̓ǂݍ���
	for (int i = 0; i < ExternalDataManager::DECIMAL_BASE; i++)
	{
		//����i�̉摜�f�[�^�̓ǂݍ���
		const int IMAGE_HANDLE = Image::Load(ExternalDataManager::GetFilePathNumberImage(i));
		assert(IMAGE_HANDLE != Image::ERROR_CORD);
	}

	//�摜�̓ǂݍ���
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathResultSceneImage(ExternalDataManager::RESULT_SCENE_IMAGE::END_GAME);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::Load(FILE_PATH);																				//�摜�̃n���h���ԍ�
	assert(IMAGE_HANDLE != Image::ERROR_CORD);

	//�`��ʒu�̐ݒ�
	transform_.position_	= DirectX::XMVectorSet(0.6f, -0.5f, 0, 0);		//�`��ʒu
	transform_.scale_		= DirectX::XMVectorSet(0.9f, 0.9f, 0, 0);		//�g��T�C�Y
}

//�X�V
void EndGameButton::Update()
{
	if (pResultScene_ == nullptr) return;	//���U���g�V�[���̃|�C���^��������΍X�V�������s��Ȃ�

	//�N���b�N����Ă��邩���m�F���A����Ă��Ȃ���΍X�V�������s��Ȃ�
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathResultSceneImage(ExternalDataManager::RESULT_SCENE_IMAGE::END_GAME);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																			//�摜�̃n���h���ԍ�
	Image::SetTransform(IMAGE_HANDLE, transform_);
	if (!Image::IsClicked(IMAGE_HANDLE))	return;

	//�Q�[���I��
	pResultScene_->EndGame();
}

//�`��
void EndGameButton::Draw()
{
	//�`��
	const std::string	FILE_PATH		= ExternalDataManager::GetFilePathResultSceneImage(ExternalDataManager::RESULT_SCENE_IMAGE::END_GAME);	//�摜�ւ̃p�X
	const int			IMAGE_HANDLE	= Image::GetHandle(FILE_PATH);																			//�摜�̃n���h���ԍ�
	Image::SetTransform(IMAGE_HANDLE, transform_);
	Image::Draw(IMAGE_HANDLE);
}

//���
void EndGameButton::Release()
{
}