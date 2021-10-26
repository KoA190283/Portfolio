#pragma once
#include "../Engine/IUserInterface.h"
#include "PlayScene.h"

//残り回数画像
class RemainingCountImage : public IUserInterface
{
private:

	PlayScene* pPlayScene_;	//プレイシーンのポインタ

public:

	RemainingCountImage();
	RemainingCountImage(PlayScene* pPlayScene);

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;
};