#include "ResultScene.h"

#include "../Engine/SceneManager.h"
#include "../BackGround.h"
#include "../ExternalDataManager.h"
#include "EndGameButton.h"
#include "ResultBoardImage.h"
#include "ResultScoreImage.h"
#include "TitleBackButton.h"

//コンストラクタ
ResultScene::ResultScene(IGameObject* parent)
	: IGameObject(parent, "ResultScene")
{
}

//初期化
void ResultScene::Initialize()
{
	//背景の初期化
	BackGround::Initialize();

	//リザルトシーンで使用するデータの初期化
	ExternalDataManager::InitializeResultSceneData();

	//UIのインスタンスを生成しリストに追加
	userInterfaceList_.push_back(new EndGameButton(this));
	userInterfaceList_.push_back(new ResultBoardImage);
	userInterfaceList_.push_back(new ResultScoreImage);
	userInterfaceList_.push_back(new TitleBackButton(this));

	//UIの初期化
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Initialize();
	}
}

//シーンを変更する
void ResultScene::ChangeSceneTitle() const
{
	SceneManager* pSceneManager = (SceneManager*)pParent_;
	pSceneManager->ChangeScene(SCENE_ID::TITLE);
}

//更新
void ResultScene::Update()
{
	//UIの更新
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Update();
	}
}

//描画
void ResultScene::Draw()
{
	//背景描画
	BackGround::Draw();			

	//UIの描画
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Draw();
	}
}

//解放
void ResultScene::Release()
{
}
