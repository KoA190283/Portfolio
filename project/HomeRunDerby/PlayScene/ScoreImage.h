#pragma once
#include "../Engine/IUserInterface.h"
#include "PlayScene.h"

//スコア画像
class ScoreImage : public IUserInterface
{
private:

	PlayScene* pPlayScene_;	//プレイシーンのポインタ

public:

	ScoreImage();
	ScoreImage(PlayScene* pPlayScene);

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;
};