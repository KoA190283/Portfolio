#pragma once
#include "../Engine/IUserInterface.h"
#include "PlayScene.h"

//ホームラン時画像
class HomeRunImage : public IUserInterface
{
private:

	PlayScene* pPlayScene_;	//プレイシーンのポインタ

public:

	HomeRunImage();
	HomeRunImage(PlayScene* pPlayScene);

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;
};