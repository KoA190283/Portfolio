#include "TitleScene.h"

#include "../Engine/SceneManager.h"
#include "../Engine/Global.h"
#include "../BackGround.h"
#include "../ExternalDataManager.h"
#include "EasyButton.h"
#include "HardButton.h"
#include "NormalButton.h"
#include "TitleImage.h"

//コンストラクタ
TitleScene::TitleScene(IGameObject* parent)
	: IGameObject(parent, "TitleScene")
{
}

//初期化
void TitleScene::Initialize()
{
	//タイトルシーンで使用するデータの初期化
	ExternalDataManager::InitializeTitleSceneData();

	//背景の初期化
	BackGround::Initialize();

	//UIのインスタンスを生成しリストに追加
	userInterfaceList_.push_back(new TitleImage);
	userInterfaceList_.push_back(new EasyButton(this));
	userInterfaceList_.push_back(new NormalButton(this));
	userInterfaceList_.push_back(new HardButton(this));

	//UIの初期化
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Initialize();
	}
}

//シーンを変更する
void TitleScene::ChangeNextScene() const
{
	SceneManager* pSceneManager = (SceneManager*)pParent_;
	pSceneManager->ChangeScene(SCENE_ID::PLAY);
}

//更新
void TitleScene::Update()
{
	//UIの更新
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		pUserInterface->Update();
	}
}

//描画
void TitleScene::Draw()
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
void TitleScene::Release()
{
	//UIの解放
	for (IUserInterface* pUserInterface : userInterfaceList_)
	{
		SAFE_RELEASE(pUserInterface);
	}
	userInterfaceList_.clear();
}
