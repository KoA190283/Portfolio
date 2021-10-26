#include "TitleScene.h"

#include "../Engine/SceneManager.h"
#include "../Engine/Global.h"
#include "../BackGround.h"
#include "../ExternalDataManager.h"
#include "EasyButton.h"
#include "HardButton.h"
#include "NormalButton.h"
#include "TitleImage.h"

//�R���X�g���N�^
TitleScene::TitleScene(IGameObject* parent)
	: IGameObject(parent, "TitleScene")
{
}

//������
void TitleScene::Initialize()
{
	//�^�C�g���V�[���Ŏg�p����f�[�^�̏�����
	ExternalDataManager::InitializeTitleSceneData();

	//�w�i�̏�����
	BackGround::Initialize();

	//UI�̃C���X�^���X�𐶐������X�g�ɒǉ�
	userInterfaceList_.push_back(new TitleImage);
	userInterfaceList_.push_back(new EasyButton(this));
	userInterfaceList_.push_back(new NormalButton(this));
	userInterfaceList_.push_back(new HardButton(this));

	//UI�̏�����
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Initialize();
	}
}

//�V�[����ύX����
void TitleScene::ChangeNextScene() const
{
	SceneManager* pSceneManager = (SceneManager*)pParent_;
	pSceneManager->ChangeScene(SCENE_ID::PLAY);
}

//�X�V
void TitleScene::Update()
{
	//UI�̍X�V
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Update();
	}
}

//�`��
void TitleScene::Draw()
{
	//�w�i�`��
	BackGround::Draw();

	//UI�̕`��
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Draw();
	}
}

//���
void TitleScene::Release()
{
	//UI�̉��
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		SAFE_RELEASE(pUserInterface);
	}
	userInterfaceList_.clear();
}
