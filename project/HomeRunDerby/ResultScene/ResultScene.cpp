#include "ResultScene.h"

#include "../Engine/SceneManager.h"
#include "../BackGround.h"
#include "../ExternalDataManager.h"
#include "EndGameButton.h"
#include "ResultBoardImage.h"
#include "ResultScoreImage.h"
#include "TitleBackButton.h"

//�R���X�g���N�^
ResultScene::ResultScene(IGameObject* parent)
	: IGameObject(parent, "ResultScene")
{
}

//������
void ResultScene::Initialize()
{
	//�w�i�̏�����
	BackGround::Initialize();

	//���U���g�V�[���Ŏg�p����f�[�^�̏�����
	ExternalDataManager::InitializeResultSceneData();

	//UI�̃C���X�^���X�𐶐������X�g�ɒǉ�
	userInterfaceList_.push_back(new EndGameButton(this));
	userInterfaceList_.push_back(new ResultBoardImage);
	userInterfaceList_.push_back(new ResultScoreImage);
	userInterfaceList_.push_back(new TitleBackButton(this));

	//UI�̏�����
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Initialize();
	}
}

//�V�[����ύX����
void ResultScene::ChangeSceneTitle() const
{
	SceneManager* pSceneManager = (SceneManager*)pParent_;
	pSceneManager->ChangeScene(SCENE_ID::TITLE);
}

//�X�V
void ResultScene::Update()
{
	//UI�̍X�V
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Update();
	}
}

//�`��
void ResultScene::Draw()
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
void ResultScene::Release()
{
}
