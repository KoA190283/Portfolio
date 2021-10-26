#pragma once
#include "../Engine/IUserInterface.h"
#include "TitleScene.h"

//難しい　モード選択ボタン
class HardButton : public IUserInterface
{
private:

	TitleScene* pTitleScene_;	//タイトルシーンのポインタ

public:

	HardButton();
	HardButton(TitleScene* pTitleScene);

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;
};
