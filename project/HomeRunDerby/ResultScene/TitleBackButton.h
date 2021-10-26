#pragma once
#include "../Engine/IUserInterface.h"
#include "ResultScene.h"

//タイトルバックボタン
class TitleBackButton : public IUserInterface
{
private:

	ResultScene* pResultScene_;	//リザルトシーンのポインタ

public:

	TitleBackButton();
	TitleBackButton(ResultScene* pResultScene);

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//解放
	void Release() override;
};