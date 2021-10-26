#include "SceneManager.h"

#include "../PlayScene/PlayScene.h"
#include "../ResultScene/ResultScene.h"
#include "../TitleScene/TitleScene.h"
#include "Image.h"
#include "Model.h"

SceneManager::SceneManager(IGameObject* parent)
	: IGameObject(parent, "SceneManager"), currentSceneID_(SCENE_ID::TITLE), nextSceneID_(SCENE_ID::TITLE)
{
}

SceneManager::~SceneManager()
{
}

//初期化
void SceneManager::Initialize()
{
	//タイトルシーンを開始画面に
	currentSceneID_ = SCENE_ID::TITLE;
	nextSceneID_	= currentSceneID_;
	Instantiate<TitleScene>(this);
}

//シーン切り替え
void SceneManager::ChangeScene(SCENE_ID nextSceneId)
{
	nextSceneID_ = nextSceneId;
}

//更新
void SceneManager::Update()
{
	//現在のシーンと次のシーンが同じであればなにもしない
	if (currentSceneID_ == nextSceneID_) return;

	//現在のシーンを終わらせる
	Model::Release();		//モデルデータの解放
	Image::Release();		//イメージデータの解放
	ReleaseAllChildren();	//シーンオブジェクト以下の全解放

	//次のシーンを開始
	switch (nextSceneID_)
	{
	case SCENE_ID::TITLE : Instantiate<TitleScene>(this) ; break;	//タイトルシーン
	case SCENE_ID::PLAY  : Instantiate<PlayScene>(this)  ; break;	//プレイシーン
	case SCENE_ID::RESULT: Instantiate<ResultScene>(this); break;	//リザルトシーン
	}

	//現在のシーンを次のシーンにする
	currentSceneID_ = nextSceneID_;
}

//描画
void SceneManager::Draw()
{

}

//解放
void SceneManager::Release()
{
}

