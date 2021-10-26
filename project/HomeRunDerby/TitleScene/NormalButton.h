#pragma once
#include "../Engine/IUserInterface.h"
#include "TitleScene.h"

//普通　モード選択ボタン
class NormalButton : public IUserInterface
{
private:

	TitleScene* pTitleScene_;	//タイトルシーンのポインタ

public:

	NormalButton();
	NormalButton(TitleScene* pTitleScene);

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;
};