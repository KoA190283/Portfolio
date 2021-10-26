#pragma once
#include "../Engine/IUserInterface.h"
#include "PlayScene.h"

//ボード画像
class BordImage : public IUserInterface
{
private:

	PlayScene* pPlayScene_;	//プレイシーンのポインタ

public:

	BordImage();
	BordImage(PlayScene* pPlayScene_);

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;
};