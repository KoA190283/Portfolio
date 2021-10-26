#pragma once
#include "IGameObject.h"

//シーンのID
enum class SCENE_ID
{
	TITLE = 0,		//タイトルシーン
	PLAY,			//プレイシーン
	RESULT,		//リザルトシーン
	MAX,
};

//シーンの遷移を管理するクラス
class SceneManager :public IGameObject
{
private:

	SCENE_ID currentSceneID_;	//現在のシーンID
	SCENE_ID nextSceneID_;		//次のシーンID

public:

	SceneManager(IGameObject* parent);
	~SceneManager();

	//初期化
	void Initialize() override;

	//シーン切り替え
	//引数:nextSceneId		次のシーンのID
	void ChangeScene(SCENE_ID nextSceneId);

private:

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;

};

