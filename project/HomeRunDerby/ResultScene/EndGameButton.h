#pragma once
#include "../Engine/IUserInterface.h"
#include "ResultScene.h"

//ゲーム終了ボタン
class EndGameButton : public IUserInterface
{
private:

	ResultScene* pResultScene_;	//リザルトシーンのポインタ

public:

	EndGameButton();
	EndGameButton(ResultScene* pResultScene);

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;
};